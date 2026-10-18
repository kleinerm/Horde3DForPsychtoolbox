function makeHorde
% Rebuild the Horde3DCore MEX-File from its source code Horde3DCore.cpp
%
% Requirements: Needs all relevant Horde3D SDK header files and libraries
% inside the local subfolder HordeEngineSDK/
%
% Needs a recent version of Psychtoolbox-3 (V3.0.22 or later) installed.
%
% May need additional Horder runtime libraries installed on your system.
%
% Usage:
%
% 1. Change Matlab/Octave working directory to the location of this
%    makeHorde.m file.
%
% 2. Call as: clear all; makeHorde
%
% --> Updated MEX file should appear inside the directory for the currently
%     running operating system and target environment.
%
% Written by Mario Kleiner, 25.7.2009.
% Updated 1.4.2012 in preparation of public release.
% Updated 19.9.2015 for current Horde 1.0.0-Beta 5 git master and VR HMD
% rendering support, as well as for OSX 10.10 build environment.
% Updated 6.10.2025 For Octave 10 and later on macOS. Some prep for Apple Silicon.
% Updated 17.10.2026 for upgrade to Horde 2.1.0 git master. Build on macOS and Windows.

if IsWin
    if ~IsWin(1)
        error('Building Horde3DCore for 32-Bit Windows is no longer supported.');
    end

    % Windows 64-Bit build for Matlab:
    mex -v Horde3DCore.cpp -I.\HordeEngineSDK -L.\HordeEngineSDK -lHorde3D -lHorde3DUtils
    movefile(['Horde3DCore.' mexext], ['HordeWin64/Horde3DCore.' mexext]);
end

if IsLinux
    % GNU/Linux build: Works for both Matlab and GNU/Octave:
    % Note: During build-time linking, copies of the Horde libraries
    % are used if they are in the arch specific folder, otherwise the
    % fallback is to system-wide installed libraries.
    % During *runtime* however, system-wide installed libraries are
    % used atm., e.g., in /usr/local/lib etc.
    %
    % Therefore it makes sense to install libraries system wide only,
    % and only use the per-arch-folder libraries in HordeLinux[ARM]64/
    % if there are specific circumstances enforcing this.
    if IsARM
        % 64-Bit ARM:
        mex -v Horde3DCore.cpp -I./HordeEngineSDK -L./HordeLinuxARM64 -lHorde3D -lHorde3DUtils
        movefile(['Horde3DCore.' mexext], ['HordeLinuxARM64/Horde3DCore.' mexext]);
    else
        % 64-Bit Intel:
        mex -v Horde3DCore.cpp -I./HordeEngineSDK -L./HordeLinux64 -lHorde3D -lHorde3DUtils
        movefile(['Horde3DCore.' mexext], ['HordeLinux64/Horde3DCore.' mexext]);
    end
end

if IsOSX
    % OS/X build:
    if IsOctave
        mex -v Horde3DCore.cpp -I./HordeEngineSDK "-Wno-deprecated-declarations -mmacosx-version-min='10.13'" "-Wl,-headerpad_max_install_names,-F/Library/Frameworks/,-framework,Horde3D,-framework,Horde3DUtils"
        osxsetoctaverpath('Horde3DCore', './');
    else
        mex -v Horde3DCore.cpp -I./HordeEngineSDK LDFLAGS="\$LDFLAGS -F/Library/Frameworks -framework Horde3D -framework Horde3DUtils"
    end

    system(['install_name_tool -change /Users/kleinerm/projects/Horde3D/Horde3D/Source/Horde3DEngine/Horde3D.framework/Versions/A/Horde3D @loader_path/Horde3D.framework/Versions/A/Horde3D Horde3DCore.' mexext]);
    system(['install_name_tool -change /Users/kleinerm/projects/Horde3D/Horde3D/Source/Horde3DUtils/Horde3DUtils.framework/Versions/A/Horde3DUtils @loader_path/Horde3DUtils.framework/Versions/A/Horde3DUtils Horde3DCore.' mexext]);

    if IsARM
        movefile(['Horde3DCore.' mexext], ['HordeOSXARM64/Horde3DCore.' mexext]);
    else
        movefile(['Horde3DCore.' mexext], ['HordeOSX64/Horde3DCore.' mexext]);
    end
end

return
