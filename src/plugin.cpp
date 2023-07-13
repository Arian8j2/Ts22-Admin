#define __USE_MINGW_ANSI_STDIO 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <teamspeak/public_definitions.h>
#include <teamspeak/public_rare_definitions.h>
#include <teamspeak/public_errors.h>
#include <ts3_functions.h>

#include "plugin.h"

#define PLUGIN_API_VERSION 26
#define INFODATA_BUFSIZE 128

TS3Functions g_Ts3Functions;
char* g_pPluginID;

// required functions
const char* ts3plugin_name(){
    return "Ts22 Admin";
}

const char* ts3plugin_version(){
    return "0.1";
}

int ts3plugin_apiVersion() {
    return PLUGIN_API_VERSION;
}

const char* ts3plugin_author(){
    return "NotAriaN";
}

const char* ts3plugin_description(){
    return "plugin for server admins actions (especially for ts22.ir admins xD), enjoy‌!";
}

void ts3plugin_setFunctionPointers(const struct TS3Functions funcs){
    g_Ts3Functions = funcs;
}

int ts3plugin_init(){
    return 0;
}

void ts3plugin_shutdown(){
    if (g_pPluginID) {
        free(g_pPluginID);
        g_pPluginID = NULL;
    }
}

void ts3plugin_registerPluginID(const char* id){
    const size_t sz = strlen(id) + 1;
    g_pPluginID = (char*)malloc(sz * sizeof(char));
    strncpy(g_pPluginID, id, sz);
}

void ts3plugin_freeMemory(void* data) {
    free(data);
}

/* Helper function to create a menu item */
static struct PluginMenuItem* createMenuItem(enum PluginMenuType type, int id, const char* text, const char* icon) {
    struct PluginMenuItem* menuItem = (struct PluginMenuItem*)malloc(sizeof(struct PluginMenuItem));
    menuItem->type = type;
    menuItem->id = id;
    strncpy(menuItem->text, text, PLUGIN_MENU_BUFSZ);
    strncpy(menuItem->icon, icon, PLUGIN_MENU_BUFSZ);
    return menuItem;
}

#define BEGIN_CREATE_MENUS(x) const size_t sz = x + 1; size_t n = 0; *menuItems = (struct PluginMenuItem**)malloc(sizeof(struct PluginMenuItem*) * sz);
#define CREATE_MENU_ITEM(a, b, c, d) (*menuItems)[n++] = createMenuItem(a, b, c, d);
#define END_CREATE_MENUS (*menuItems)[n++] = NULL;

void ts3plugin_initMenus(struct PluginMenuItem*** menuItems, char** menuIcon){
    BEGIN_CREATE_MENUS(2);
    CREATE_MENU_ITEM(PLUGIN_MENU_TYPE_CHANNEL, MENU_ID_MOVE_THERE, "Move Clients There", "right.png");
    CREATE_MENU_ITEM(PLUGIN_MENU_TYPE_CHANNEL, MENU_ID_MOVE_HERE, "Move Clients To My Channel", "left.png");
    END_CREATE_MENUS;

    *menuIcon = (char*)malloc(PLUGIN_MENU_BUFSZ * sizeof(char));
    strncpy(*menuIcon, "logo.png", PLUGIN_MENU_BUFSZ);
}

static void MoveClients(uint64& serverConnectionHandlerID, uint64& SourceChannel, uint64& DstChannel){
    anyID* pClientList;
    g_Ts3Functions.getChannelClientList(serverConnectionHandlerID, SourceChannel, &pClientList);

    for(anyID* pClient = pClientList; *pClient != 0; pClient++){
        g_Ts3Functions.requestClientMove(
            serverConnectionHandlerID,
            *pClient,
            DstChannel,
            "",
            NULL
        );
    }
}

void ts3plugin_onMenuItemEvent(uint64 serverConnectionHandlerID, enum PluginMenuType type, int menuItemID, uint64 selectedItemID){
    anyID Clid;
    g_Ts3Functions.getClientID(serverConnectionHandlerID, &Clid);

    uint64 Cid;
    g_Ts3Functions.getChannelOfClient(serverConnectionHandlerID, Clid, &Cid);

    switch(menuItemID){
        case MENU_ID_MOVE_THERE:{
            MoveClients(serverConnectionHandlerID, Cid, selectedItemID);
            break;
        }

        case MENU_ID_MOVE_HERE:{
            MoveClients(serverConnectionHandlerID, selectedItemID, Cid);
            break;
        }
    }
}

const char* ts3plugin_infoTitle() {
	return "Ts22 Admin";
}

void ts3plugin_infoData(uint64 serverConnectionHandlerID, uint64 id, enum PluginItemType type, char** data){
    *data = (char*)malloc(INFODATA_BUFSIZE * sizeof(char));
    switch(type){
        case PLUGIN_CLIENT:{
            int Cldbid;
            g_Ts3Functions.getClientVariableAsInt(
                serverConnectionHandlerID,
                id, CLIENT_DATABASE_ID,
                &Cldbid
            );

            snprintf(*data, INFODATA_BUFSIZE,
                "Client ID --> [b]%llu[/b]\nClient DB ID --> [b]%d[/b]", id, Cldbid);

            break;
        }

        case PLUGIN_CHANNEL:{
            snprintf(*data, INFODATA_BUFSIZE, "Channel ID --> [b]%llu[/b]", id);
            break;
        }

        case PLUGIN_SERVER:{
            unsigned char aVariables[] = {
                CLIENT_IS_CHANNEL_COMMANDER,
                CLIENT_TYPE
            };
            unsigned int aResults[sizeof(aVariables)/sizeof(char)] = {0};

            anyID* pClientList;
            g_Ts3Functions.getClientList(serverConnectionHandlerID, &pClientList);

            for(anyID* pClient = pClientList; *pClient != 0; pClient++){
                for(int i=0; i < static_cast<int>(sizeof(aVariables)/sizeof(char)); i++){
                    int Buffer;
                    unsigned int Error = g_Ts3Functions.getClientVariableAsInt(
                        serverConnectionHandlerID,
                        *pClient, aVariables[i],
                        &Buffer
                    );

                    if(Error != ERROR_ok){
                        data = NULL;
                        return;
                    }

                    if(Buffer == 1)
                        aResults[i]++;
                }
            }

            snprintf(*data, INFODATA_BUFSIZE,
                "Commander clients --> [b]%u[/b]\nServer queries --> [b]%u[/b]", aResults[0], aResults[1]);
            break;
        }

        default:
			data = NULL;
			return;
    }
}
