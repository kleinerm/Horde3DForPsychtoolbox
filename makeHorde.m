function makeHorde
% Rebuild the Horde3DCore MEX-File from its source code Horde3DCore.cpp
%
% Requirements: Needs all relevant Horde3D SDK header files and libraries
% inside the local subfolder HordeEngineSDK/
%
% Needs a recent version of Psychtoolbox-3 (V3.0.14 or later) installed.
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

if IsWin
    if ~IsWin(1)
        error('Building Horde3DCore for 32-Bit Windows is no longer supported.');
    end

    % Windows 64-Bit build for Matlab:
    mex -v Horde3DCore.cpp -I.\HordeEngineSDK -L.\HordeEngineSDK -lHorde3D_vc10 -lHorde3DUtils_vc10
    movefile(['Horde3DCore.' mexext], ['HordeWin64/Horde3DCore.' mexext]);
end

if IsLinux
    % GNU/Linux build: Works for both Matlab and GNU/Octave 3.2.x:
    if IsLinux(1)
        % 64-Bit: Use system installed Horde SDK
        mex -v Horde3DCore.cpp -I./HordeEngineSDK -lHorde3D -lHorde3DUtils
        if IsOctave
            striplibsfrommexfile('Horde3DCore.mex');
        end
        movefile(['Horde3DCore.' mexext], ['HordeLinux64/Horde3DCore.' mexext]);
    else
        % 32-Bit: Use bundled libraries in SDK folder
        mex -v Horde3DCore.cpp -I./HordeEngineSDK -L./HordeEngineSDK -lHorde3D -lHorde3DUtils
        movefile(['Horde3DCore.' mexext], ['HordeLinux32/Horde3DCore.' mexext]);
    end
end

if IsOSX
    % OS/X build:
    if IsOctave
        mex -v Horde3DCore.cpp -I./HordeEngineSDK "-Wl, -mmacosx-version-min='10.11'" "-Wl,-headerpad_max_install_names,-F/System/Library/Frameworks/,-F/Library/Frameworks/ -framework Horde3D -framework Horde3DUtils,-syslibroot,'/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk' -mmacosx-version-min='10.11'"
        osxsetoctaverpath('Horde3DCore');
    else
        mex -v Horde3DCore.cpp -I./HordeEngineSDK LDFLAGS="\$LDFLAGS -framework Horde3D -framework Horde3DUtils"
    end
    
    if Is64Bit
        movefile(['Horde3DCore.' mexext], ['HordeOSX64/Horde3DCore.' mexext]);
    else
        movefile(['Horde3DCore.' mexext], ['HordeOSX32/Horde3DCore.' mexext]);
    end
end

return
