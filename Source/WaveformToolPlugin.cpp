#include "IllustratorSDK.h"
#include "WaveformToolPlugin.h"
#include "SDKErrors.h"
#include "App.h"

Plugin* AllocatePlugin(SPPluginRef pluginRef) {
    return new WaveformToolPlugin(pluginRef);
}

void FixupReload(Plugin* plugin) {
	WaveformToolPlugin::FixupVTable((WaveformToolPlugin*) plugin);
}

WaveformToolPlugin::WaveformToolPlugin(SPPluginRef pluginRef) : Plugin(pluginRef), fResourceManagerHandle(NULL), fShutdownApplicationNotifier(NULL), fNotifySelectionChanged(NULL) {
    this->app = new App();
	strncpy(fPluginName,kWaveformToolPluginName,kMaxStringLength);
}

WaveformToolPlugin::~WaveformToolPlugin() {
    delete this->app;
}

ASErr WaveformToolPlugin::Message(char* caller, char* selector, void *message){
	ASErr error = kNoErr;
	try {
		error = Plugin::Message(caller, selector, message);
	}
	catch (ai::Error& ex) {
		error = ex;
	}
	catch (...) {
		error = kCantHappenErr;
	}

	if(error) {
		if(error == kUnhandledMsgErr) {
			if(strcmp(caller, kCallerAIAnnotation) == 0) {
				if(strcmp(selector, kSelectorAIDrawAnnotation) == 0) {
					error = DrawAnnotator((AIAnnotatorMessage*)message);
					aisdk::check_ai_error(error);
				}
				else if(strcmp(selector, kSelectorAIInvalAnnotation) == 0) {
					error = InvalAnnotator((AIAnnotatorMessage*)message);
					aisdk::check_ai_error(error);
				}				
			}
		}
		else {
			Plugin::ReportError(error, caller, selector, message);
		}
	}	
	return error;
}

ASErr WaveformToolPlugin::Notify( AINotifierMessage *message ) {
	ASErr error = kNoErr;
	if (message->notifier == fShutdownApplicationNotifier) {
		if(fResourceManagerHandle != NULL) {
			error = sAIUser->DisposeCursorResourceMgr(fResourceManagerHandle);
			fResourceManagerHandle = NULL;
		}
	}	
	else if (message->notifier == fNotifySelectionChanged) {
        // Get the bounds of the current document view.
        AIRealRect viewBounds = {0, 0, 0, 0};
        error = sAIDocumentView->GetDocumentViewBounds(NULL, &viewBounds);
        aisdk::check_ai_error(error);
        // Invalidate the entire document view bounds.
        error = InvalidateRect(viewBounds);
        aisdk::check_ai_error(error);
	}
	return error;
}

ASErr WaveformToolPlugin::StartupPlugin(SPInterfaceMessage *message) {
	ASErr error = kNoErr;
	ai::int32 pluginOptions = 0;	
	
	error = Plugin::StartupPlugin(message);
	if(error) goto error;
	
	error = sAIPlugin->GetPluginOptions(message->d.self, &pluginOptions);
	if(error) goto error;
	
	error = sAIPlugin->SetPluginOptions(message->d.self, pluginOptions | kPluginWantsResultsAutoSelectedOption );
	if(error) goto error;
	
	error = AddTools(message);
	if(error) goto error;

	error = AddAnnotator(message);
	if(error) goto error;

	error = AddMenus(message);
	if(error) goto error;

	error = AddNotifier(message);
	if(error) goto error;
    
    sAITimer->AddTimer(message->d.self,(char *)"info.tokisato.Waveform.timer",2,this->app->getTimer()); // 30fps
    this->app->lock();

error:
	return error;
}

ASErr WaveformToolPlugin::GoMenuItem(AIMenuMessage* message) {
	ASErr error = kNoErr;
	if(message->menuItem == this->fAboutPluginMenu) {
		// Pop this plug-in's about box.
		SDKAboutPluginsHelper aboutPluginsHelper;
		aboutPluginsHelper.PopAboutBox(message, "About WaveformTool", kSDKDefAboutSDKCompanyPluginsAlertString);
	}	
	return error;
}

ASErr WaveformToolPlugin::AddMenus(SPInterfaceMessage *message) {
	ASErr error = kNoErr;
	// Add a menu item to the About SDK Plug-ins menu group.
	SDKAboutPluginsHelper aboutPluginsHelper;
	error = aboutPluginsHelper.AddAboutPluginsMenuItem(message, 
        kSDKDefAboutSDKCompanyPluginsGroupName,
        ai::UnicodeString(kSDKDefAboutSDKCompanyPluginsGroupNameString),
        "WaveformTool...",
        &fAboutPluginMenu);
	return error;
}

ASErr WaveformToolPlugin::AddTools(SPInterfaceMessage *message) {
    AIAddToolData toolData;
    toolData.title   = "Waveform";
    toolData.tooltip = "Waveform";
    toolData.darkIconResID   = 19000;
    toolData.normalIconResID = 19001;
    toolData.sameGroupAs   = kNoTool;
    toolData.sameToolsetAs = kNoTool;
    
    AIErr error = sAITool->AddTool(message->d.self,toolData.title,&toolData,kToolWantsToTrackCursorOption,&fToolHandle);
    return error;
}

ASErr WaveformToolPlugin::AddNotifier(SPInterfaceMessage *message) {
	ASErr result = kNoErr;
	try {
		result = sAINotifier->AddNotifier(fPluginRef,"WaveformToolPlugin",kAIApplicationShutdownNotifier, &fShutdownApplicationNotifier);
		aisdk::check_ai_error(result);
		result = sAINotifier->AddNotifier(fPluginRef,"WaveformToolPlugin",kAIArtSelectionChangedNotifier, &fNotifySelectionChanged);

		aisdk::check_ai_error(result);
	}
	catch(ai::Error &ex) {
		result = ex;
	}
	catch(...) {
		result = kCantHappenErr;
	}
	return result;
}


ASErr WaveformToolPlugin::GoTimer(AITimerMessage *message) {
    this->app->draw(message);
    return kNoErr; 
}

ASErr WaveformToolPlugin::SelectTool(AIToolMessage *message) {
    this->app->unlock();
    return kNoErr;
}

ASErr WaveformToolPlugin::DeselectTool(AIToolMessage *message) {
    this->app->lock();
    return kNoErr;
}

ASErr WaveformToolPlugin::AddAnnotator(SPInterfaceMessage *message) {
	ASErr result = kNoErr;
	try {
		result = sAIAnnotator->AddAnnotator(message->d.self,"WaveformTool Annotator",&fAnnotatorHandle);
		aisdk::check_ai_error(result);
		result = sAIAnnotator->SetAnnotatorActive(fAnnotatorHandle, false);
		aisdk::check_ai_error(result);
	}
	catch(ai::Error &ex) {
		result = ex;
	}
	catch(...) {
		result = kCantHappenErr;
	}
	return result;
}

ASErr WaveformToolPlugin::DrawAnnotator(AIAnnotatorMessage *message) {
	ASErr result = kNoErr;
	try {
		// Get the string to display in annotator.
		ai::UnicodeString pointStr;
		result = this->GetPointString(fEndPoint,pointStr);
		aisdk::check_ai_error(result);

		AIPoint annotatorPoint;
		result = sAIDocumentView->ArtworkPointToViewPoint(NULL,&fEndPoint,&annotatorPoint);

		// Move 5 points right and 5 points up.
		annotatorPoint.h += 5;
		annotatorPoint.v -= 5;

		// Find cursor bound rect.
		AIRect annotatorRect;
		result = sAIAnnotatorDrawer->GetTextBounds(message->drawer,pointStr,&annotatorPoint,false,annotatorRect);
		aisdk::check_ai_error(result);

		// Draw a filled rectangle, the following R, G and B values combined makes light yellow.
		unsigned short red = 65000;
		unsigned short green = 65000;
		unsigned short blue = 40000;
		AIRGBColor yellowFill = {red,green,blue};
		sAIAnnotatorDrawer->SetColor(message->drawer,yellowFill);
		result = sAIAnnotatorDrawer->DrawRect(message->drawer,annotatorRect,true);
		aisdk::check_ai_error(result);

		// Draw black outline, 0 for R, G and B makes black.
		unsigned short black = 0;
		AIRGBColor blackFill = {black,black,black};
		sAIAnnotatorDrawer->SetColor(message->drawer,blackFill);
		sAIAnnotatorDrawer->SetLineWidth(message->drawer,0.5);
		result = sAIAnnotatorDrawer->DrawRect(message->drawer,annotatorRect,false);
		aisdk::check_ai_error(result);

		// Draw cursor text.
		result = sAIAnnotatorDrawer->SetFontPreset(message->drawer,kAIAFSmall);
		aisdk::check_ai_error(result);

		result = sAIAnnotatorDrawer->DrawTextAligned(message->drawer,pointStr,kAICenter,kAIMiddle,annotatorRect);
		aisdk::check_ai_error(result);

		// Save old rect
		oldAnnotatorRect = annotatorRect;
	}
	catch(ai::Error &ex) {
		result = ex;
	}
	return result;
}

ASErr WaveformToolPlugin::InvalAnnotator(AIAnnotatorMessage *message) {
	ASErr result = sAIAnnotator->InvalAnnotationRect(NULL,&oldAnnotatorRect);
	return result;
}

ASErr WaveformToolPlugin::InvalidateRect(const AIRect &invalRect) {
	ASErr result = kNoErr;
	try {		
		// Invalidate the rect bounds so it is redrawn.
		result = sAIAnnotator->InvalAnnotationRect(NULL,&invalRect);
		aisdk::check_ai_error(result);
	}
	catch(ai::Error &ex) {
		result = ex;
	}
	return result;
}

ASErr WaveformToolPlugin::InvalidateRect(const AIRealRect &invalRealRect) {
	ASErr result = kNoErr;
	try {		
		// invalRealRect is in artwork coordinates, convert to view 
		// coordinates for AIAnnotatorSuite::InvalAnnotationRect.
		AIRect invalRect;
		result = this->ArtworkBoundsToViewBounds(invalRealRect,invalRect);
		aisdk::check_ai_error(result);

		// Invalidate the rect bounds so it is redrawn.
		result = sAIAnnotator->InvalAnnotationRect(NULL,&invalRect);
		aisdk::check_ai_error(result);
	}
	catch(ai::Error &ex) {
		result = ex;
	}
	return result;
}

ASErr WaveformToolPlugin::ArtworkBoundsToViewBounds(const AIRealRect &artworkBounds, AIRect &viewBounds) {
	ASErr result = kNoErr;
	try {
		AIRealPoint tlArt, brArt;
		tlArt.h = artworkBounds.left;
		tlArt.v = artworkBounds.top;
		brArt.h = artworkBounds.right;
		brArt.v = artworkBounds.bottom;

		// Convert artwork coordinates to view coordinates.
		AIPoint tlView, brView;
		result = sAIDocumentView->ArtworkPointToViewPoint(NULL,&tlArt,&tlView);
		aisdk::check_ai_error(result);
		result = sAIDocumentView->ArtworkPointToViewPoint(NULL,&brArt,&brView);
		aisdk::check_ai_error(result);

		viewBounds.left = tlView.h;
		viewBounds.top = tlView.v;
		viewBounds.right = brView.h;
		viewBounds.bottom = brView.v;
	}
	catch(ai::Error &ex) {
		result = ex;
	}
	return result;
}

ASErr WaveformToolPlugin::GetPointString(const AIRealPoint &point, ai::UnicodeString &pointStr) {
	ASErr result = kNoErr;
	try {
		ASInt32 precision = 2;
		ai::UnicodeString horiz, vert;
		ai::NumberFormat numFormat;

		horiz = numFormat.toString((float)point.h,precision,horiz);
		vert = numFormat.toString((float)-point.v,precision,vert);

		pointStr.append(ai::UnicodeString("h: ")
				.append(horiz)
				.append(ai::UnicodeString(", v: ")
				.append(vert)));
	}
	catch(ai::Error &ex) {
		result = ex;
	}
	return result;
}

ASErr WaveformToolPlugin::PostStartupPlugin() {
    AIErr result = kNoErr;
    result = sAIUser->CreateCursorResourceMgr(fPluginRef,&fResourceManagerHandle);
    return result;
}
