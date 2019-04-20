#include "IllustratorSDK.h"
#include "WaveformToolSuites.h"

extern "C" {
	AIMenuSuite                 *sAIMenu = NULL;
	AIToolSuite                 *sAITool = NULL;
	AIUnicodeStringSuite        *sAIUnicodeString = NULL;
	AIPluginSuite               *sAIPlugin = NULL;
	SPBlocksSuite               *sSPBlocks = NULL;
	AIArtSuite                  *sAIArt = NULL;
	AIRealMathSuite             *sAIRealMath = NULL;
	AIPathSuite                 *sAIPath = NULL;
	AIPathStyleSuite            *sAIPathStyle = NULL;
	AIAnnotatorSuite            *sAIAnnotator = NULL;
	AIAnnotatorDrawerSuite      *sAIAnnotatorDrawer = NULL;
	AIDocumentViewSuite         *sAIDocumentView = NULL;
	AIStringFormatUtilsSuite    *sAIStringFormatUtils = NULL;
	AIArtStyleSuite             *sAIArtStyle = NULL;
	AIArtboardSuite             *sAIArtboard = NULL;
	AITimerSuite                *sAITimer = NULL;
}

ImportSuite gImportSuites[] = {
	kAIMenuSuite, kAIMenuSuiteVersion, &sAIMenu,
	kAIToolSuite, kAIToolVersion, &sAITool,
	kAIUnicodeStringSuite, kAIUnicodeStringVersion, &sAIUnicodeString,
	kAIPluginSuite, kAIPluginSuiteVersion, &sAIPlugin,
	kSPBlocksSuite, kSPBlocksSuiteVersion, &sSPBlocks,
	kAIArtSuite, kAIArtVersion, &sAIArt,
	kAIRealMathSuite, kAIRealMathVersion, &sAIRealMath,	
	kAIPathSuite, kAIPathSuiteVersion, &sAIPath,
	kAIPathStyleSuite, kAIPathStyleSuiteVersion, &sAIPathStyle,
	kAIAnnotatorSuite, kAIAnnotatorSuiteVersion, &sAIAnnotator,
	kAIAnnotatorDrawerSuite, kAIAnnotatorDrawerSuiteVersion, &sAIAnnotatorDrawer,
	kAIDocumentViewSuite, kAIDocumentViewSuiteVersion, &sAIDocumentView,
	kAIStringFormatUtilsSuite, kAIStringFormatUtilsSuiteVersion, &sAIStringFormatUtils,
	kAIArtStyleSuite, kAIArtStyleSuiteVersion, &sAIArtStyle,
	kAIArtboardSuite, kAIArtboardSuiteVersion, &sAIArtboard,
	kAITimerSuite, kAITimerSuiteVersion, &sAITimer,
	nil, 0, nil
};
