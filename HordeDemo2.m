function HordeDemo2(windowed)
% Demonstration of usage of Horde3D engine from within Psychtoolbox.
%
% Usage:
%
% HordeDemo2([windowed=0]);
% - The optional 'windowed' parameter, if set to a non-zero value, will
% display the rendering in a window. Otherwise it will be shown in a
% fullscreen display.
%
% Use mouse buttons, mouse moves and space key to change parameters.
% Press ESCape key to exit.
%

% History:
% 15-Sep-2015  mk  Update for Horde 1.0.0 Beta 5 git master + PTB 3.0.12.

% This global HE is defined by HorderHelper('Initialize') and contains all
% important Horde engine constants conveniently defined as a struct:
global HE;

% Check if Psychtoolbox is properly installed and ready:
AssertOpenGL;

% Windowed or not?
if nargin < 1
    % Default to fullscreen:
    windowed = 0;
end

% 'windowed' == 1: Show in 800 x 600 pixels window. Else show fullscreen:
if windowed
    rect = [ 0 0 1000 800 ] ;
else
    rect = [] ;
end

% Setup keys on keyboard that trigger actions:
KbName('UnifyKeyNames');
space = KbName('space');
escape = KbName('ESCAPE');

% Wait for all keys to be released:
KbReleaseWait;

% Prepare Psychtoolbox for OpenGL low-level 3D rendering operations:
InitializeMatlabOpenGL(0,0);

% Open window on screen 0, the main display:
screenid = 0;

% The imgpipe flag and associated code demonstrates how to use Horde with
% the PTB imaging pipeline for more special stuff.
% For basic stuff you can leave it at imgpipe = 0 and ignore
% all associated code for imgpipe settings other than zero.
imgpipe = 0;

% msamples, if set to a non-zero value, allows to enable anti-aliasing by
% multisampling. Higher numbers = more samples per pixel = higher quality
% and lower speed.
msamples = 0;

if imgpipe == 0
    win = Screen('OpenWindow', screenid, 0, rect, [], [], [], msamples);
    woff = win;
else
    % Use Psychtoolbox imaging pipeline. This needs special setup...
    PsychImaging('PrepareConfiguration');
    PsychImaging('AddTask', 'General', 'UseVirtualFramebuffer');
    % Show final image left/right mirrored:
    PsychImaging('AddTask', 'AllViews', 'FlipHorizontal');
    win  = PsychImaging('OpenWindow', screenid, 0, rect);
    woff = win;

    if imgpipe == 1
        % Setup a offscreen window 'woff' as rendertarget:
        woff = Screen('OpenOffscreenWindow', win);
    end
end

% Query windows size w x h pixels for later Horde renderer setup:
[w,h] = Screen('WindowSize', win);

Screen('TextSize', win, 24);

% Get initial timestamp and setup defaults:
ts = Screen('Flip', win);
t = ts;

% Switch to 3D mode:
Screen('BeginOpenGL', woff);

try
    % Initialize Horde3DCore and define all HE.xxxx constants for use:
    HordeHelper('Initialize');
catch
    sca;
    psychrethrow(psychlasterror);
    return;
end

try

    % display horde's version string
    %ver = Horde3DCore('HordeVersion');
    %ver;

    % check if there is an error
    % Horde3DCore('GetError');
    disp('Checcking for Messages...');
    [msg, lvl, tme] = Horde3DCore('GetMessage');
    fprintf('msg=%s, level=%d, time=%d \n', msg, lvl, tme);

    % Add all required resources:

    % Rendering pipeline - Defines all basic rendering settings for redrawing a frame:
    PipeRes = Horde3DCore('AddResource', HE.H3DResTypes.Pipeline, 'pipelines/forward.pipeline.xml');
    % HDR Pipe: PipeRes = Horde3DCore('AddResource', HE.H3DResTypes.Pipeline, 'pipelines/hdr.pipeline.xml');

    % Environment: 3D SceneGraph defining the sky, ground, sphere etc.
    envRes = Horde3DCore('AddResource', HE.H3DResTypes.SceneGraph, 'models/sphere/sphere.scene.xml');

    % Environment: 3D SceneGraph defining the sky, ground, sphere etc.
    skyRes = Horde3DCore('AddResource', HE.H3DResTypes.SceneGraph, 'models/skybox/skybox.scene.xml');

    % Scene graph with the "Knight" - the 3D character model:
    knightRes = Horde3DCore('AddResource', HE.H3DResTypes.SceneGraph, 'models/knight/knight.scene.xml');

    % Two animation ressources: Motion tracks (keyframe based) for the two knight animation moves:
    knightAnim1Res = Horde3DCore('AddResource', HE.H3DResTypes.Animation, 'animations/knight_order.anim');
    knightAnim2Res = Horde3DCore('AddResource', HE.H3DResTypes.Animation, 'animations/knight_attack.anim');

    % Another body model definition and its animation track for the walking "Agent Smith" guys:
    characterRes = Horde3DCore('AddResource', HE.H3DResTypes.SceneGraph, 'models/man/man.scene.xml', 0 );
    characterWalkRes = Horde3DCore('AddResource', HE.H3DResTypes.Animation, 'animations/man.anim' );%, 0 );

    moonRes = Horde3DCore('AddResource', HE.H3DResTypes.SceneGraph, 'models/sphere/sphere.scene.xml');

    % Load all the added resources into engine. 'Content' is the filesystem
    % path to the folder that contains all needed animation resources. All
    % resource names specified above via 'AddResource' are relative to this
    % basepath. Our example 'Content' means there is a subfolder 'Content' in
    % the current working directory which contains all the subfolders and data
    % files which define the scene/model/animation tracks etc.:
    Horde3DCore('LoadResources', [fileparts(mfilename('fullpath')) filesep 'Content']);

    disp('Checking Resource knight...');
    kname = Horde3DCore('GetResName', knightRes);
    ktype = Horde3DCore('GetResType', knightRes);
    knelems = Horde3DCore('GetResElemCount', knightRes, 1);
    fprintf('Resource Name:%s, Type:%d, NoOfElems:%d, IsResLoaded:\n', kname, ktype, knelems);
    Horde3DCore('IsResLoaded', knightRes);
    
    % Add a camera to the scene: The behaviour and basic rendering parameters
    % of the camera are defined by the Pipeline resource 'PipeRes', and it has
    % the name 'MyCamera'. The name can be anything...
    %
    % 'camera' is the handle that you can use to modify camera settings later
    % on:
    camera = Horde3DCore('AddCamera', 'MyCamera', PipeRes);

    % Add scenegraph with the environment scene 'envRes' to the root of the
    % scenegraph, return handle 'env' to it for later manipulations:
    env = Horde3DCore('AddNodes', HE.H3DRootNode, envRes);

    % Setup rigid position, orientation and scale of the 'env' environment
    % scenegraph relative to its parentNode, the H3DRootNode:
    % Translation (x,y,z) = [0, -20, 0] units.
    % Rotation (rx,ry,rz) = [0, 0, 0] degrees euler angles.
    % Scaling  (sx,sy,sz) = [20, 20, 20] units.
    Horde3DCore('SetNodeTransform', env, 0, -20, 0, 0, 0, 0, 20, 20, 20);
    Horde3DCore('SetNodeActivation', env, 0);

    sky = Horde3DCore('AddNodes', HE.H3DRootNode, skyRes);
    Horde3DCore('SetNodeTransform', sky, 0, 0, 0, 0, 0, 180, 210, 50, 210 );

    % Add scenegraph with the human body model 'knightRes' to the root of the
    % scenegraph, return handle 'knight' to it for later manipulations:
    knight = Horde3DCore('AddNodes', HE.H3DRootNode, knightRes);

    par = Horde3DCore('GetNodeParent', knight);
    pname = Horde3DCore('GetNodeType', par);
    fprintf('Parent of Knight Node(%d) is: %d\n', Horde3DCore('GetNodeType', knight), pname);

    fprintf('Moon Resource created, IsResLoaded?\n');
    Horde3DCore('IsResLoaded', moonRes);
    moon = Horde3DCore('AddNodes', HE.H3DRootNode, moonRes);
    Horde3DCore('SetNodeTransform', moon, -20, -40, 0, 0, 0, 0, 10, 10, 10);

    % This call disables software skinning and uses GPU accelerated
    % hardware skinning instead:
    Horde3DCore('SetNodeParami', knight, HE.H3DModel.SWSkinningI, 0);

    % Set rigid start position, orientation, scale of the knight as well:
    kx = 0; ky = 0; kz = 0; khead = 180;
    Horde3DCore('SetNodeTransform', knight, kx, ky, kz, 0, khead, 0, 0.1, 0.1, 0.1 );

    % Assign keyframe animation tracks defined by 'knightAnim1Res' and
    % 'knightAnim2Res' as motion tracks 0 and 1 to the 'knight' body model.
    % Animation is non-additive, as set by the zero flag at the end. We set the
    % name of the first node to apply to '' so it applies to the first node:
    Horde3DCore('SetupModelAnimStage', knight, 0, knightAnim1Res, '', 0 ) ;
    Horde3DCore('SetupModelAnimStage', knight, 1, knightAnim2Res, '', 0 ) ;

    % Retrieve handle to the model node representing the joint of the right hand of the
    % knight, ie., the subnode of 'knight' with name 'Bip01_R_Hand' of type
    % HE.H3DNodeTypes.Joint. This allows to manipulate that 'handjoint' by
    % its handle 'handjoint':
    handjoint = Horde3DCore( 'FindNodes', knight, 'Bip01_R_Hand', HE.H3DNodeTypes.Joint ) ; %#ok<NASGU>

    % Add fifty additional scenegraph nodes, each with the same
    % characterRes model ("Agent Smith") and animation track
    % characterWalkRes assigned, so we get fifty instances guy(1) to
    % guy(50) that can be animated independently:
    r = 6 ;
    p = 0 ;
    guycount = 100 ;
    for i=1:guycount
        % Add i'th guy, store handle in guy(i):
        guy(i) = Horde3DCore('AddNodes', HE.H3DRootNode, characterRes); %#ok<AGROW>

        % Assign characterWalkRes with its walking animation:
        Horde3DCore('SetupModelAnimStage', guy(i), 0, characterWalkRes, '', 0);

        % Position the guy in a location on a circly around the 'knight'
        tx = r*cos((i + p)/guycount * 2 * pi);
        tz = r*sin((i + p)/guycount * 2 * pi);
        ang = -(i + p)/guycount * 360;
        Horde3DCore('SetNodeTransform', guy(i), tx, 0, tz, 0, ang, 0, 2, 2, 2 );

        % Build matrices which contain all the info computed above, one matrix
        % column per guy. This allows quick "batch-updating" of the node
        % parameters and animations of many guys at once for fast animation
        % of many models:
        %
        % xform(1, i) = Handle of i'th guy node.
        % xform(2:4, i) = New [tx, ty, tz] translation of guy.
        % xform(5:7, i) = New [rx, ry, rz] euler rotation angles of guy.
        % xform(8:10, i) = New [sx, sy, sz] scales of guy.
        xform(:, i) = [ guy(i), tx, -1, tz, 0, ang, 0, 2, 2, 2 ]'; %#ok<AGROW>

        % anims works the same for animation parameters:
        % anims(1, i) = handle of guy(i) to which to apply change.
        % anims(2, i) = idx of animation track to change.
        % anims(3, i) = New virtual simulation time for that animation track in seconds.
        % anims(4, i) = New animation weight for linear combination with other active tracks.
        anims(:, i) = [ guy(i),  0, 0 * 24.0, 1.0 ]'; %#ok<AGROW>
    end

    % Add a light node which defines 1 light source:
    light = Horde3DCore('AddLightNode', HE.H3DRootNode, 'Light1', 0, 'LIGHTING', 'SHADOWMAP');

    % Position the light in space:
    Horde3DCore('SetNodeTransform', light, 20, 15, 10, -60, 0, 0, 1, 1, 1 );

    % Set light specific parameters:
    Horde3DCore('SetNodeParamf', light, HE.H3DLight.RadiusF, 0, 30 );
    Horde3DCore('SetNodeParamf', light, HE.H3DLight.FovF, 0, 90 );
    Horde3DCore('SetNodeParami', light, HE.H3DLight.ShadowMapCountI, 1 );
    Horde3DCore('SetNodeParamf', light, HE.H3DLight.ShadowMapBiasF, 0, 0.01 );
    Horde3DCore('SetNodeParamf', light, HE.H3DLight.ColorF3, 0, 1.0 );
%    Horde3DCore('SetNodeParamf', light, HE.H3DLight.Col_G, 0.8 );  %TODO: calculate the above color param
%    Horde3DCore('SetNodeParamf', light, HE.H3DLight.Col_B, 0.7 );

    % Define the viewport for rendering: The target rectangle of the target
    % window into which rendering should go: (xs, ys, widht, height):
    Horde3DCore('SetupViewport', camera, 0, 0, w, h);

    % Set camera intrinsics for aspect ratio correct rendering in a window of
    % width 'w' and height 'h' and 45 deg. field of view. Objects closer
    % than 0.1 units to the camera plane or farther away than 1000 units
    % shall be clipped to save some rendering time:
    Horde3DCore('SetupCameraView', camera, 45.0, w / h, 0.1, 1000.0);

    % Set initial camera position and orientation:
    cx = 15; cy = 23; cz = 89; crx = -17; cry = 15;
    Horde3DCore('SetNodeTransform', camera, cx, cy, cz, crx, cry, 0, 1, 1, 1);

    % Animation loop: Run until keypress:
    modetxt = 'Press SPACE to switch control modes, mouse buttons to change parameters.';
    fc = 0;
    tstart = GetSecs;
    weight = 0.0;
    mmode = 0;
    [mxo, myo] = GetMouse;
    oldisdown = 0;

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
                mmode = mod(mmode + 1, 4);
                switch(mmode)
                    case 0,
                        modetxt = 'Camera moves: LMB = x/y, OMB = z';
                    case 1,
                        modetxt = 'Camera rotation: LMB = heading / pitch';
                    case 2,
                        modetxt = 'Knight moves: LMB = x/z, OMB = Heading';
                    case 3,
                        modetxt = 'Morph weights moves: LMB = Change.';
                end
            end
        end
        oldisdown = isdown;

        % Query mouse position and button state:
        [mx, my, mb] = GetMouse;

        if any(mb)
            % Deltas for mouse relative movement:
            mdx = mx - mxo;
            mdy = my - myo;

            % Some mouse button pressed: Trigger action depending on mmode:
            switch(mmode)
                case 0,
                    % Camera translation:
                    mdx = mdx * 0.01;
                    mdy = mdy * 0.01;
                    if ~mb(1)
                        % Forward / Backward:
                        cz = cz + mdy;
                    else
                        % Left/Right, Up/Down:
                        cx = cx + mdx;
                        cy = cy - mdy;
                    end

                    % Apply new camera node transform:
                    Horde3DCore('SetNodeTransform', camera, cx, cy, cz, crx, cry, 0, 1, 1, 1);
                case 1,
                    mdx = mdx * 0.1;
                    mdy = mdy * 0.1;

                    % Camera rotation:
                    crx = crx + mdy;
                    cry = cry - mdx;

                    % Apply new camera node transform:
                    Horde3DCore('SetNodeTransform', camera, cx, cy, cz, crx, cry, 0, 1, 1, 1);
                case 2,
                    % Knight translation and rotation:
                    if mb(1)
                        % Left/Right, Forward/Backward:
                        mdx = mdx * 0.01;
                        mdy = mdy * 0.01;
                        kx = kx + mdx;
                        kz = kz + mdy;
                    else
                        khead = khead + mdx;
                    end

                    Horde3DCore('SetNodeTransform', knight, kx, ky, kz, 0, khead, 0, 0.1, 0.1, 0.1 );
                case 3,
                    % Control morph weight between knights trajectories:
                    if mb(1)
                        weight = min(max((weight + mdx * 0.001), 0), 1);
                    end
                otherwise
                    warning('Unknown mmode state! Fix your script!!'); %#ok<WNTAG>
            end
        end
        mxo = mx;
        myo = my;

        % Update animation for simulationtime t - ts, render scene:
        tSimulation = t - ts;

        % Update animation for knight:
        Horde3DCore('SetModelAnimParams', knight, 0, tSimulation * 24.0, weight);
        Horde3DCore('SetModelAnimParams', knight, 1, tSimulation * 24.0, 1 - weight);

        % Update rotation angle of knights right hand joint:
        [hposX, hposY, hposZ, hrotX, hrotY, hrotZ, hsX, hsY, hsZ] = Horde3DCore('GetNodeTransform', handjoint);
        hrotX = 90 * sin(tSimulation * 0.1);
        hrotZ = 90 * sin(tSimulation * 0.1);
        Horde3DCore('SetNodeTransform', handjoint, hposX, hposY, hposZ, hrotX, hrotY, hrotZ, hsX, hsY, hsZ);

        Horde3DCore('SetNodeTransform', moon, -20+cos(tSimulation)*50, -10+sin(tSimulation)*50, 0, 0, 0, 0, 10, 10, 10);

        % Update position of the circle walking guys on their circle:
        p = tSimulation * 35.0 / 360.0 * 50.0;

        % Vectorized update of xform matrix for all guys to reposition and reorient them quickly:
        i=1:size(xform, 2);
        tx = r*cos((i + p)/50 * 2 * pi);
        ty = r*sin((i + p)/50 * 4 * pi)+5;
        tz = r*sin((i + p)/50 * 2 * pi);
        ang = -(i + p)/50 * 360;
        xform([2,3,4,6], :) = [ tx; ty; tz; ang ];
        dispstr=['tx=', num2str(tx(10)), ', ty=', num2str(ty(10)), ', tz=', num2str(tz(10)), ', yang=', num2str(ang(10))];
        disp(dispstr);

        % Batch-Update rigid transformations:
        Horde3DCore('SetNodeTransforms', xform);

        % Vectorized update of anims animation tracks simulated time for all guys:
        anims(3, :) = tSimulation * 35.0;

        % Batch-Submit new animation parameters:
        Horde3DCore('SetModelsAnimParameters', anims);

        % Move camera
        cx = 15+sin(tSimulation)*10; cy = 23; cz = 89; crx = -17; cry = 15;
        Horde3DCore('SetNodeTransform', camera, cx, cy, cz, crx, cry, 0, 1, 1, 1);

        % Move the light in space:
        Horde3DCore('SetNodeTransform', light, 20+sin(tSimulation)*50, 15+sin(tSimulation)*50, 10, -60, 0, 0, 1, 1, 1 );

        % Render updated models and scene into framebuffer, using camera
        % 'camera':
        Horde3DCore('Render', camera);

        % We're done with rendering for this frame:
        Horde3DCore('FinalizeFrame');

        % Store debug output (if any) to log file:
        Horde3DCore('DumpMessages');

        % Done with 3D rendering:
        Screen('EndOpenGL', win);

        if imgpipe == 1
            % For the fun of it, blit the offscreen window 'woff' twict
            % into PTB's framebuffer, using different methods:
            Screen('CopyWindow', woff, win);
            Screen('DrawTexture', win, woff, [], [0 0 w/2 h/2]);
        end

        % Print some status text onto the screen:
        DrawFormattedText(win, [modetxt, sprintf('\nMorphWeight = %f\n', weight)], 0, 25, 255);

        % Show rendered frame at next vertical retrace, retrieve true stimulus
        % onset timestamp 't':
        t = Screen('Flip', win, [], [], 0);

        % Prepare next 3D render-pass:
        Screen('BeginOpenGL', woff);

        % Repeat...
        fc = fc + 1;
    end

    avgfps = fc / (t - tstart);
    fprintf('Average update rate was %f FPS.\n', avgfps);

    % Shutdown Horde3DCore:
    Horde3DCore('Shutdown');

    % Disable 3D rendering:
    Screen('EndOpenGL', win);

    % Close window, release all ressources:
    Screen('CloseAll');

catch
    % Shutdown Horde3DCore:
    Horde3DCore('Shutdown');

    sca;
    psychrethrow(psychlasterror);
end

% Done.
return;
