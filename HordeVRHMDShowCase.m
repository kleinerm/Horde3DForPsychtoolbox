function HordeVRHMDShowCase(forceMonoDisplay, perEyeRender)
% Demonstration of usage of Horde3D engine from within Psychtoolbox with
% supported VR Head mounted displays (HMDs), e.g., the Oculus Rift DK2.
%
% Usage:
%
% HordeVRHMDShowCase([forceMonoDisplay=0][, perEyeRender=0]);
%
% Use mouse buttons, mouse moves and space key to change parameters.
% Press ESCape key to exit.
%

% History:
% 15-Sep-2015  mk  Started. Derived from HordeStereoDemo.

% This global HE is defined by HorderHelper('Initialize') and contains all
% important Horde engine constants conveniently defined as a struct:
global HE;

if nargin < 1 || isempty(forceMonoDisplay)
    forceMonoDisplay = 0;
end

if nargin < 2 || isempty(perEyeRender)
    perEyeRender = 0;
end

gs = 0.05;

% Check if Psychtoolbox is properly installed and ready, set defaults:
PsychDefaultSetup(2);

% Setup keys on keyboard that trigger actions:
space = KbName('space');
escape = KbName('ESCAPE');
LeftShift = KbName('LeftShift');

% Wait for all keys to be released:
KbReleaseWait;

% Prepare Psychtoolbox for OpenGL low-level 3D rendering operations:
InitializeMatlabOpenGL;

% Open window on secondary screen:
screenid = max(Screen('Screens'));

% Use Psychtoolbox imaging pipeline. This needs special setup...
PsychImaging('PrepareConfiguration');
if ~forceMonoDisplay
    hmd = PsychVRHMD('AutoSetupHMD', 'Tracked3DVR', 'LowPersistence');
else
    hmd = [];
end

if ~isempty(hmd)
    PsychVRHMD('SetHSWDisplayDismiss', hmd, -1);
else
    screenid = max(0, screenid - 1);
end
[win, winRect] = PsychImaging('OpenWindow', screenid, 0);
[w,h] = Screen('WindowSize', win);

% Set textsize to 24 pixels:
Screen('TextSize', win, 24);

% Get initial timestamp and setup defaults:
ts = Screen('Flip', win);
t = ts;

% Switch to 3D mode for Horde3D setup:
Screen('BeginOpenGL', win);

try
    % Initialize Horde3DCore and define all HE.xxxx constants for use:
    HordeHelper('Initialize');
    Horde3DCore('SetOption', HE.H3DOptions.MaxNumMessages, 100000);
    Horde3DCore('SetOption', HE.H3DOptions.MaxLogLevel, 2);

    %Horde3DCore('SetOption', HE.H3DOptions.LoadTextures, 0);
    %Horde3DCore('SetOption', HE.H3DOptions.DebugViewMode, 1);
catch %#ok<CTCH>
    sca;
    psychrethrow(psychlasterror);
    return;
end

try
    % Add all required resources:

    % Rendering pipeline - Defines all basic rendering settings for redrawing a frame:
    PipeRes = Horde3DCore('AddResource', HE.H3DResTypes.Pipeline, 'pipelines/forward.pipeline.xml');
    % HDR Pipe: PipeRes = Horde3DCore('AddResource', HE.H3DResTypes.Pipeline, 'pipelines/hdr.pipeline.xml');
    Horde3DCore('DumpMessages');

    % Environment: The sky and ground:
    skyRes = Horde3DCore('AddResource', HE.H3DResTypes.SceneGraph, 'models/skybox/skybox.scene.xml');
    Horde3DCore('DumpMessages');

    % Load all assets from filesystem and instantiate corresponding nodes:
    baseassetpath = [fileparts(mfilename('fullpath')) filesep 'StarTrek' filesep 'models' filesep];
    assets = dir([ baseassetpath '*' ]);
    assetres = zeros(1, length(assets));
    for i = 1:length(assets)
        if assets(i).isdir
            assetdir = [ baseassetpath assets(i).name filesep];
            assetfile = [assetdir assets(i).name '.scene.xml'];
            if ~exist(assetfile, 'file')
                assetres(i) = -1;
                continue;
            end
            
            assetfile = ['models' filesep assets(i).name filesep assets(i).name '.scene.xml'];
            fprintf('Loading scene: %s\n', assetfile);
            nodename{i} = assets(i).name;
            assetres(i) = Horde3DCore('AddResource', HE.H3DResTypes.SceneGraph, assetfile);
            Horde3DCore('DumpMessages');
            str = Horde3DCore('GetMessage');
            if ~isempty(str)
                disp(str);
            end            
        end
    end

    % Cut down to available scene nodes:
    validres = find(assetres >= 0);
    assetres = assetres(validres);
    newnodename = {};

    for i = 1:length(validres)
        newnodename{end+1} = nodename{validres(i)};
    end
    nodename = newnodename

%    enterpriseRes = Horde3DCore('AddResource', HE.H3DResTypes.SceneGraph, 'models/Enterprise_A_Hull_corrected/EnterpriseA.scene.xml');
%    enterpriseRes = Horde3DCore('AddResource', HE.H3DResTypes.SceneGraph, 'models/Enterprise/Enterprise.scene.xml');
%    Horde3DCore('DumpMessages');

%    MagneticConstrictionRes = Horde3DCore('AddResource', HE.H3DResTypes.SceneGraph, 'models/MagneticConstriction/MagneticConstriction.scene.xml');
%    Horde3DCore('DumpMessages');

%     ImpulseEngineRoomRes = Horde3DCore('AddResource', HE.H3DResTypes.SceneGraph, 'models/ImpulseEngineRoom/ImpulseEngineRoom.scene.xml');
%     Horde3DCore('DumpMessages');
% 
%     FDeckRes = Horde3DCore('AddResource', HE.H3DResTypes.SceneGraph, 'models/F-Deck-PrimaryHull/F-Deck-PrimaryHull.scene.xml');
%     Horde3DCore('DumpMessages');
% 
%     GDeckRes = Horde3DCore('AddResource', HE.H3DResTypes.SceneGraph, 'models/G-Deck-PrimaryHull/G-Deck-PrimaryHull.scene.xml');
%     Horde3DCore('DumpMessages');
% 
%     HDeckRes = Horde3DCore('AddResource', HE.H3DResTypes.SceneGraph, 'models/HDeck/HDeck.scene.xml');
%     Horde3DCore('DumpMessages');
% 
%     IDeckRes = Horde3DCore('AddResource', HE.H3DResTypes.SceneGraph, 'models/I-Deck-PrimaryHull/I-Deck-PrimaryHull.scene.xml');
%     Horde3DCore('DumpMessages');

    % Load all the added resources into engine. 'Content' is the filesystem
    % path to the folder that contains all needed animation resources. All
    % resource names specified above via 'AddResource' are relative to this
    % basepath. Our example 'Content' means there is a subfolder 'Content' in
    % the current working directory which contains all the subfolders and data
    % files which define the scene/model/animation tracks etc.:
    baseRes = [fileparts(mfilename('fullpath')) filesep 'Content' '|' ...
               fileparts(mfilename('fullpath')) filesep 'StarTrek'];
    Horde3DCore('LoadResources', baseRes);
    Horde3DCore('DumpMessages');
    gluErrorString % Query and clear gl error on Apples fragile and buggy snowflake


    % Add a stereo camera pair to the scene: The behaviour and basic rendering parameters
    % of the cameras are defined by the Pipeline resource 'PipeRes', and they have the
    % names 'MyLeft/RightCamera'. The name can be anything...
    %
    % 'camera(1:2)' are the handles that you can use to modify camera settings later on:
    camera(1) = Horde3DCore('AddCamera', 'MyLeftCamera', PipeRes);
    camera(2) = Horde3DCore('AddCamera', 'MyRightCamera', PipeRes);

    clipFar = 100000.0 * gs / 1;
    clipNear = 0.1;

    Horde3DCore('SetNodeParamf', camera(1), HE.H3DCamera.FarPlaneF, 0, clipFar);
    Horde3DCore('SetNodeParamf', camera(2), HE.H3DCamera.FarPlaneF, 0, clipFar);

    % Enable occlusion culling: Does not help much here, but it does cut
    % the triangle count to submit to GPU in half - sadly that is not
    % enough for us to get decent performance:
    Horde3DCore('SetNodeParami', camera(1), HE.H3DCamera.OccCullingI, 0);
    Horde3DCore('SetNodeParami', camera(2), HE.H3DCamera.OccCullingI, 0);

    % Add a rock to the root of the scenegraph, return handle 'rock' to it for later manipulations:
%    rock = Horde3DCore('AddNodes', HE.H3DRootNode, rockRes);
%    Horde3DCore('SetNodeTransform', rock, 0, -20, 0, 0, 0, 0, 20, 20, 20);

%    MagneticConstriction = Horde3DCore('AddNodes', HE.H3DRootNode, MagneticConstrictionRes);
%    Horde3DCore('SetNodeTransform', MagneticConstriction, 0, 0, 100, 0, 0, 0, 1, 1, 1);

    % Some sky box, so the world has an (open) ending:
    sky = Horde3DCore('AddNodes', HE.H3DRootNode, skyRes);
    Horde3DCore('SetNodeTransform', sky, 0, 0, 0, 0, 0, 0, 24000 * gs, 24000 * gs, 24000 * gs);

    % Add resources as scene nodes to the scene graph:
    node = zeros(1, length(assetres));
    for i = 1:length(assetres)
        fprintf('Adding scene node %i.\n', i);
        node(i) = Horde3DCore('AddNodes', HE.H3DRootNode, assetres(i));
        Horde3DCore('SetNodeTransform', node(i), 0, 0, 0, 0, 0, 0, 1 * gs, 1 * gs, 1 * gs);
        if ~isempty(strfind(nodename{i}, 'Enterprise'))
           enterprise = i;     
        end
    end

    % Add a light node which defines 1 light source:
    light = Horde3DCore('AddLightNode', HE.H3DRootNode, 'Light1', 0, 'LIGHTING', 'SHADOWMAP');
    Horde3DCore('SetNodeTransform', light, 50, 15 + 50, 130, -60, 0, 0, 1, 1, 1 );

    % Set light specific parameters:
    Horde3DCore('SetNodeParamf', light, HE.H3DLight.RadiusF, 0, 30000 );
    Horde3DCore('SetNodeParamf', light, HE.H3DLight.FovF, 0, 360 );
    Horde3DCore('SetNodeParami', light, HE.H3DLight.ShadowMapCountI, 1 );
    Horde3DCore('SetNodeParamf', light, HE.H3DLight.ShadowMapBiasF, 0, 0.01 );
    %Horde3DCore('SetNodeParamf', light, HE.H3DLight.ColorF3, 0, 1.0 );
    
    % Define the viewport for rendering: The target rectangle of the target
    % window into which rendering should go: (xs, ys, width, height):
    Horde3DCore('SetupViewport', camera(1), 0, 0, w, h);
    Horde3DCore('SetupViewport', camera(2), 0, 0, w, h);

    if ~isempty(hmd)
        % Retrieve camera projection matrices for optimal rendering on the HMD:
        [projL, projR] = PsychVRHMD('GetStaticRenderParameters', hmd, clipNear, clipFar);

        % Assign them to cameras:
        Horde3DCore('SetCameraProjMat', camera(1), projL);
        Horde3DCore('SetCameraProjMat', camera(2), projR);
    else
        % Set camera intrinsics for aspect ratio correct rendering in a window of
        % width 'w' and height 'h' and 45 deg. field of view. Objects closer
        % than 0.1 units to the camera plane or farther away than 100000 units
        % shall be clipped to save some rendering time:
        Horde3DCore('SetupCameraView', camera, 45.0, w / h, clipNear, clipFar);

        % Set initial camera position and orientation:
        cx = 5; cy = 13; cz = 19; crx = -17; cry = 15;
        Horde3DCore('SetNodeTransform', camera, cx, cy, cz, crx, cry, 0, 1, 1, 1);
    end

    % Set initial camera position and orientation:
    cx = 0; cy = 80; cz = 250; crx = -17; cry = 0; heading = 0;
    cx = 0 * gs; cy = 80 * gs; cz = -7100 * gs; crx = -17; cry = 180; heading = 180;
    
    Screen('EndOpenGL', win);

    % Apply new camera node transform:
    Horde3DCore('SetNodeTransform', camera(1), cx, cy, cz, crx, cry, 0, 1, 1, 1);
    Horde3DCore('SetNodeTransform', camera(2), cx, cy, cz, crx, cry, 0, 1, 1, 1);

    modetxt = 'Press SPACE to switch control modes, mouse buttons to change parameters.';
    fc = 0;
    tstart = GetSecs;
    mmode = 0;
    [xc, yc] = RectCenter(winRect);
    SetMouse(xc,yc, screenid);
    HideCursor(screenid);
    [mxo, myo] = GetMouse(screenid);

    globalHeadPose = PsychGetPositionYawMatrix([cx, cy, cz], heading);

    observerOutside = 1;
    observerOutsideOld = -1;
    oldisdown = 0;
    tic;

    % Animation loop: Run until keypress:
    while 1
        % Keyboard query:
        [isdown, secs, keyCode] = KbCheck;
        if isdown && ~oldisdown
            if keyCode(escape)
                % ESCape pressed: Exit render loop:
                break;
            end

            if keyCode(space)
                % Space pressed: Switch mouse control mode...
                mmode = mod(mmode + 1, 2);
                switch(mmode)
                    case 0,
                        modetxt = 'Camera moves: LMB = x/y, OMB = z';
                    case 1,
                        modetxt = 'Camera rotation: LMB = heading / pitch';
                end
            end
            
            if keyCode(LeftShift)
                % Switch inside vs. outside view:
                observerOutside = 1 - observerOutside;
            end
        end
        oldisdown = isdown;

        % Query mouse position and button state:
        [mx, my, mb] = GetMouse(screenid);
        % Deltas for mouse relative movement:
        mdx = mx - mxo;
        mdy = my - myo;

        if any(mb)
            % Some mouse button pressed: Trigger action depending on mmode:
            switch(mmode)
                case 0,
                    % Camera translation:
                    mdx = mdx * 0.1;
                    mdy = mdy * 0.1;
                    if ~mb(1)
                        % Forward / Backward:
                        cz = cz + mdy;
                    else
                        % Left/Right, Up/Down:
                        cx = cx + mdx;
                        cy = cy - mdy;
                    end
                    % Apply new camera node transform:
                    Horde3DCore('SetNodeTransform', camera(1), cx, cy, cz, crx, cry, 0, 1, 1, 1);
                    Horde3DCore('SetNodeTransform', camera(2), cx, cy, cz, crx, cry, 0, 1, 1, 1);

                case 1,
                    mdx = mdx * 0.1;
                    mdy = mdy * 0.1;

                    % Camera rotation:
                    % Camera rotation:
                    crx = crx + mdy;
                    cry = cry - mdx;

                    heading = heading - mdx;
                    % Apply new camera node transform:
                    Horde3DCore('SetNodeTransform', camera(1), cx, cy, cz, crx, cry, 0, 1, 1, 1);
                    Horde3DCore('SetNodeTransform', camera(2), cx, cy, cz, crx, cry, 0, 1, 1, 1);

                otherwise
                    warning('Unknown mmode state! Fix your script!!'); %#ok<WNTAG>
            end
        end

        % Reposition mouse cursor for next drive cycle:
        SetMouse(xc,yc, screenid);
        [mxo, myo] = GetMouse(screenid);

        % Update animation for simulationtime t - ts, render scene:
        tSimulation = t - ts;

        if ~isempty(hmd)
            % Track head position and orientation, retrieve camera matrices for each eye,
            % as well as tracking state. Apply global pose transform globalHeadPose:
            if 1
                if mb(1)
                    dcz = mdy * 0.25;
                    dHeading = -mdx * 1;
                else
                    dcz = 0;
                    dHeading = 0;
                end

                if any(mb(2:3))
                    dcy = -mdy * 0.1;
                else
                    dcy = 0;
                end

                globalHeadPose = globalHeadPose * PsychGetPositionYawMatrix([0, dcy, dcz], dHeading);
            else
                globalHeadPose = PsychGetPositionYawMatrix([cx, cy, cz], heading);
            end
            
            state = PsychVRHMD('PrepareRender', hmd, globalHeadPose);

            if ~perEyeRender
                % Setup Horde3D cameras from camera view matrices:
                Horde3DCore('SetNodeTransMat', camera(1), state.cameraView{1});
                Horde3DCore('SetNodeTransMat', camera(2), state.cameraView{2});
            end
        end

        if observerOutside ~= observerOutsideOld
            observerOutsideOld = observerOutside;
            if observerOutside
                for i=1:length(node)
                    Horde3DCore('SetNodeFlags', node(i), HE.H3DNodeFlags.Inactive, 1);
                end
                Horde3DCore('SetNodeFlags', node(enterprise), 0, 1);

                Horde3DCore('SetNodeParamf', camera(1), HE.H3DCamera.FarPlaneF, 0, clipFar);
                Horde3DCore('SetNodeParamf', camera(2), HE.H3DCamera.FarPlaneF, 0, clipFar);
                if ~isempty(hmd)
                    % Retrieve camera projection matrices for optimal rendering on the HMD:
                    [projL, projR] = PsychVRHMD('GetStaticRenderParameters', hmd, clipNear, clipFar);
                    
                    % Assign them to cameras:
                    Horde3DCore('SetCameraProjMat', camera(1), projL);
                    Horde3DCore('SetCameraProjMat', camera(2), projR);
                end
            else
                for i=1:length(node)
                    Horde3DCore('SetNodeFlags', node(i), 0, 1);
                end
                Horde3DCore('SetNodeFlags', node(enterprise), HE.H3DNodeFlags.Inactive, 1);

                shortClip = 100;
                Horde3DCore('SetNodeParamf', camera(1), HE.H3DCamera.FarPlaneF, 0, clipFar / shortClip);
                Horde3DCore('SetNodeParamf', camera(2), HE.H3DCamera.FarPlaneF, 0, clipFar / shortClip);
                if ~isempty(hmd)
                    % Retrieve camera projection matrices for optimal rendering on the HMD:
                    [projL, projR] = PsychVRHMD('GetStaticRenderParameters', hmd, clipNear, clipFar / shortClip);
                    
                    % Assign them to cameras:
                    Horde3DCore('SetCameraProjMat', camera(1), projL);
                    Horde3DCore('SetCameraProjMat', camera(2), projR);
                end
            end
        end

        for renderPass = 0:(1-forceMonoDisplay)
            if ~isempty(hmd) && perEyeRender
                eyePose = PsychVRHMD('GetEyePose', hmd, renderPass, globalHeadPose);
                sbuf = eyePose.eyeIndex;
                Horde3DCore('SetNodeTransMat', camera(sbuf+1), eyePose.cameraView);
            else
                sbuf = renderPass;
            end

            Screen('SelectStereoDrawbuffer', win, sbuf);

            % Prepare next 3D render-pass:
            Screen('BeginOpenGL', win);

            % Render updated models and scene into framebuffer, using camera 'camera':
            Horde3DCore('Render', camera(sbuf+1));

            Screen('EndOpenGL', win);
        end

        % We're done with rendering for this frame:
        Horde3DCore('FinalizeFrame');
        %elapsed = toc
        % Head position tracked?
        if ~isempty(hmd) && ~bitand(state.tracked, 2)
          % Nope, user out of cameras view frustum. Tell it like it is:
          DrawFormattedText(win, 'Vision based tracking lost\nGet back into the cameras field of view!', 'center', 'center', [1 0 0]);
        end

        if isempty(hmd)
            % Print some status text onto the screen:
            DrawFormattedText(win, [modetxt, sprintf('\n[x,yz] = [%f %f %f]\n', cx, cy, cz)], 0, 0, 255);
        end

        % Show rendered frame at next vertical retrace, retrieve true stimulus
        % onset timestamp 't':
        t = Screen('Flip', win);

        % Stats for nerds:
        tricount = Horde3DCore('GetStat', HE.H3DStats.TriCount, 1);
        batchcount = Horde3DCore('GetStat', HE.H3DStats.BatchCount, 1);
%         GeoUpdateTime = Horde3DCore('GetStat', HE.H3DStats.GeoUpdateTime, 1)
%        FwdLightsGPUTime = Horde3DCore('GetStat', HE.H3DStats.FwdLightsGPUTime, 1)
%         ShadowsGPUTime = Horde3DCore('GetStat', HE.H3DStats.ShadowsGPUTime, 1)
        
        % Repeat...
        fc = fc + 1;
        stats(:, fc) = [tricount ; batchcount];
    end

    % Enable 3D rendering so we can call Horde shutdown properly:
    Screen('BeginOpenGL', win);

    % Shutdown Horde3DCore:
    Horde3DCore('Shutdown');

    % Disable 3D rendering:
    Screen('EndOpenGL', win);

    % Close window, release all ressources:
    sca;

    avgfps = fc / (t - tstart);
    fprintf('Average update rate was %f FPS.\n', avgfps);
    plot(stats(1, :));
    figure;
    plot(stats(2, :));
catch
    % Shutdown Horde3DCore:
    Horde3DCore('Shutdown');

    sca;
    psychrethrow(psychlasterror);
end

% Done.
return;
