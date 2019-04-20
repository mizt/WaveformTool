#ifndef __WAVEFORMTOOLPLUGIN_H__
#define __WAVEFORMTOOLPLUGIN_H__

#include "Plugin.hpp"
#include "SDKDef.h"
#include "SDKAboutPluginsHelper.h"
#include "WaveformToolID.h"
#include "WaveformToolSuites.h"

#import "App.h"

Plugin* AllocatePlugin(SPPluginRef pluginRef);
void FixupReload(Plugin* plugin);

class WaveformToolPlugin : public Plugin {
    
    public:

        WaveformToolPlugin(SPPluginRef pluginRef);
        ~WaveformToolPlugin();
        FIXUP_VTABLE_EX(WaveformToolPlugin, Plugin);

        virtual ASErr Notify(AINotifierMessage *message);

    protected:

        virtual ASErr Message(char *caller, char *selector, void *message);
        virtual ASErr StartupPlugin(SPInterfaceMessage *message);
        virtual ASErr GoMenuItem(AIMenuMessage *message);
    
        virtual ASErr SelectTool(AIToolMessage *message);
        virtual ASErr DeselectTool(AIToolMessage *message);
        virtual ASErr GoTimer(AITimerMessage *message);
    
    private:

        AIMenuItemHandle fAboutPluginMenu;
        AIToolHandle fToolHandle;
        AIRealPoint fStartingPoint;
        AIRealPoint fEndPoint;
        AIRect oldAnnotatorRect;
        AIAnnotatorHandle fAnnotatorHandle;
        AINotifierHandle fShutdownApplicationNotifier;
        AINotifierHandle fNotifySelectionChanged;
    
        AIResourceManagerHandle fResourceManagerHandle;
        ASErr AddTools(SPInterfaceMessage *message);
        ASErr AddMenus(SPInterfaceMessage *message);
        ASErr AddAnnotator(SPInterfaceMessage *message);
        ASErr AddNotifier(SPInterfaceMessage *message);
        ASErr DrawAnnotator(AIAnnotatorMessage *message);
        ASErr InvalAnnotator(AIAnnotatorMessage *message);
        ASErr GetPointString(const AIRealPoint &point, ai::UnicodeString &pointStr);
        ASErr PostStartupPlugin();
        ASErr ArtworkBoundsToViewBounds(const AIRealRect &artworkBounds, AIRect &viewBounds);
        ASErr InvalidateRect(const AIRealRect &invalRealRect);
        ASErr InvalidateRect(const AIRect &invalRect);
    
        App *app;
};

#endif
