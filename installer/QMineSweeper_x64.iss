; Script generated by the Inno Script Studio Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "QMineSweeper (x64)"
#define MyAppVersion GetFileVersion("QMineSweeper.exe")
#define MyAppPublisher "Tyler Lewis"
#define MyAppURL "https://github.com/tlewiscpp/QMineSweeper"
#define MyAppExeName "QMineSweeper.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{05DD558F-0AF2-423C-B2FF-01395BFE7BE5}
AppName={#MyAppName}
AppVerName={#MyAppName} {#MyAppVersion}
VersionInfoVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
LicenseFile=..\resources\licenses\LICENSE
OutputDir=..\installer\
OutputBaseFilename=QMineSweeper_Win_x64
SetupIconFile=..\resources\QMineSweeper.ico
UninstallDisplayIcon=..\resources\QMineSweeper.ico
Compression=lzma
SolidCompression=yes
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Dirs]
Name: "{app}\audio"
Name: "{app}\bearer"
Name: "{app}\iconengines"Name: "{app}\imageformats"
Name: "{app}\mediaservice"
Name: "{app}\platforms"
Name: "{app}\playlistformats"
Name: "{app}\resources"
Name: "{app}\styles"
Name: "{app}\translations"

[Files]
Source: "..\cmake-build-release\QMineSweeper.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\cmake-build-release\d3dcompiler_47.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\cmake-build-release\libEGL.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\cmake-build-release\libGLESV2.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\cmake-build-release\opengl32sw.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\cmake-build-release\Qt5Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\cmake-build-release\Qt5Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\cmake-build-release\Qt5Multimedia.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\cmake-build-release\Qt5Network.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\cmake-build-release\Qt5Svg.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\cmake-build-release\Qt5Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\cmake-build-release\vc_redist.x64.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\cmake-build-release\audio\qtaudio_wasapi.dll"; DestDir: "{app}\audio"; Flags: ignoreversion
Source: "..\cmake-build-release\audio\qtaudio_windows.dll"; DestDir: "{app}\audio"; Flags: ignoreversion
Source: "..\cmake-build-release\bearer\qgenericbearer.dll"; DestDir: "{app}\bearer"; Flags: ignoreversion
Source: "..\cmake-build-release\iconengines\qsvgicon.dll"; DestDir: "{app}\iconengines"; Flags: ignoreversion
Source: "..\cmake-build-release\imageformats\qgif.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "..\cmake-build-release\imageformats\qicns.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "..\cmake-build-release\imageformats\qico.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "..\cmake-build-release\imageformats\qjpeg.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "..\cmake-build-release\imageformats\qsvg.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "..\cmake-build-release\imageformats\qtga.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "..\cmake-build-release\imageformats\qtiff.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "..\cmake-build-release\imageformats\qwbmp.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "..\cmake-build-release\imageformats\qwebp.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "..\cmake-build-release\mediaservice\dsengine.dll"; DestDir: "{app}\mediaservice"; Flags: ignoreversion
Source: "..\cmake-build-release\mediaservice\qtmedia_audioengine.dll"; DestDir: "{app}\mediaservice"; Flags: ignoreversion
Source: "..\cmake-build-release\mediaservice\wmfengine.dll"; DestDir: "{app}\mediaservice"; Flags: ignoreversion
Source: "..\cmake-build-release\platforms\qwindows.dll"; DestDir: "{app}\platforms"; Flags: ignoreversion
Source: "..\cmake-build-release\playlistformats\qtmultimedia_m3u.dll"; DestDir: "{app}\playlistformats"; Flags: ignoreversion
Source: "..\cmake-build-release\styles\qwindowsvistastyle.dll"; DestDir: "{app}\styles"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files


[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]

[Run]
;Filename: "{app}\vc_redist.x64.exe"; Parameters: "/q /passive /Q:a /c:""msiexec /q /i vcredist.msi"""; StatusMsg: "Installing VC++ Redistributables..."
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent


[Registry]
Root: HKCR; SubKey: ".qms"; ValueType: string; ValueData: "QMineSweeper game"; Flags: uninsdeletekey
Root: HKCR; SubKey: "QMineSweeper game"; ValueType: string; ValueData: "QMineSweeper game"; Flags: uninsdeletekey
Root: HKCR; SubKey: "QMineSweeper game\Shell\Open\Command"; ValueType: string; ValueData: """{app}\QMineSweeper.exe"" ""%1"""; Flags: uninsdeletekey