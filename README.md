# WaveformTool

2008 / 2019    

### Note

Only supported on macOS

#### Supported SDK

[Adobe Illustrator CC 2018 SDK](https://console.adobe.io/downloads/ai)

#### Lsinked Frameworks

* Cocoa.framework
* CoreAudio.framework
* AudioUnit.framework

#### Build Settings

Complie Souces As `Objective-C++`

#### Run Script

	mkdir -p "${CONFIGURATION_BUILD_DIR}/${PRODUCT_NAME}.aip/Contents/Resources/png"
	mkdir -p "${CONFIGURATION_BUILD_DIR}/${PRODUCT_NAME}.aip/Contents/Resources/txt"
	cp -f "./Resources/raw/WaveformTool.png" "${CONFIGURATION_BUILD_DIR}/${PRODUCT_NAME}.aip/Contents/Resources/png/WaveformTool.png"
	cp -f "./Resources/raw/WaveformTool@2x.png" "${CONFIGURATION_BUILD_DIR}/${PRODUCT_NAME}.aip/Contents/Resources/png/WaveformTool@2x.png"
	cp -f "./Resources/raw/WaveformTool@3to2x.png" "${CONFIGURATION_BUILD_DIR}/${PRODUCT_NAME}.aip/Contents/Resources/png/WaveformTool@3to2x.png"
	cp -f "./Resources/raw/WaveformToolDark.png" "${CONFIGURATION_BUILD_DIR}/${PRODUCT_NAME}.aip/Contents/Resources/png/WaveformToolDark.png"
	cp -f "./Resources/raw/WaveformToolDark@2x.png" "${CONFIGURATION_BUILD_DIR}/${PRODUCT_NAME}.aip/Contents/Resources/png/WaveformToolDark@2x.png"
	cp -f "./Resources/raw/WaveformToolDark@3to2x.png" "${CONFIGURATION_BUILD_DIR}/${PRODUCT_NAME}.aip/Contents/Resources/png/WaveformToolDark@3to2x.png"
	cp -f "./Resources/raw/IDToFile.txt" "${CONFIGURATION_BUILD_DIR}/${PRODUCT_NAME}.aip/Contents/Resources/txt/IDToFile.txt"
	exit

