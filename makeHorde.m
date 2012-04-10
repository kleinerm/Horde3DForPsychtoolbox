function makeHorde
% Rebuild the Horde3DCore MEX-File from its source code Horde3DCore.cpp
%
% Requirements: Needs all relevant Horde3D SDK header files and libraries
% inside the local subfolder HordeEngineSDK/
%
% Needs a recent version of Psychtoolbox-3 (V3.0.9 or later) installed.
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
%

if IsWin
    % Windows build: Works for both Matlab and GNU/Octave 3.2.x:
    mex -v Horde3DCore.cpp -I.\HordeEngineSDK -L.\HordeEngineSDK -lHorde3D_vc8 -lHorde3DUtils_vc8
    movefile(['Horde3DCore.' mexext], ['HordeWin32/Horde3DCore.' mexext]);
end

if IsLinux
    % GNU/Linux build: Works for both Matlab and GNU/Octave 3.2.x:
    if IsLinux(1)
        % 64-Bit: Use system installed Horde SDK
        mex -v Horde3DCore.cpp -I./HordeEngineSDK -lHorde3D -lHorde3DUtils
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
        mex -v Horde3DCore.cpp -I./HordeEngineSDK -W" -framework Horde3D -framework Horde3DUtils"
    else
        mex -v Horde3DCore.cpp -I./HordeEngineSDK CXXLIBS='-framework Horde3D -framework Horde3DUtils' CLIBS='-framework Horde3D -framework Horde3DUtils'
    end
    movefile(['Horde3DCore.' mexext], ['HordeOSX32/Horde3DCore.' mexext]);
end

return
