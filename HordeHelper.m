function varargout = HordeHelper(cmd, varargin)
% HordeHelper - Useful helper and utility functions for using Horde3D.
%
% Usage:
%
% HordeHelper('Initialize');
% - Initialize Horde3DCore and Horde3D engine. Call this while the target
% window for rendering is active and in 3D mode, ie., after
% Screen('BeginOpenGL', ...); was called for the window!
%
% This will also define a global struct variable 'global HE' which contains
% all interesting Horde engine constants.
%
% DEPRECATED FUNCTIONS - NO LONGER NEEDED:
%
% rendertarget = HordeHelper('GetRendertargetForWindow', win);
% - Setup onscreen or offscreen window 'win' for rendering and return the
% 'rendertarget' handle to be used after each render-pass in the call to
% HordeHelper('EndOpenGL', rendertarget);
% This is not strictly needed as long as you don't want to draw into
% offscreen windows (e.g., for image processing operations on the rendered
% scene) and as long as you don't use the Psychtoolbox imaging pipeline for
% postprocessing. Using it in the trivial case doesn't hurt however, it is
% basically a no operation then.
%
% HordeHelper('EndOpenGL', rendertarget);
% - Finalize rendering for 'rendertarget' window.
%
%

% History:
% 29.7.2009  MK  Written.
% ??.?.2010  YNK  Updated to Horde version 1.0.0 Beta4.
% 16.9.2015  MK  Updated to Horde version 1.0.0 Beta5 - Current git master.

% This global variable will hold all global Horde data structures and
% enumerations of Horde3D parameters:
global HE;

% Need access to GL structs:
global GL;

if strcmpi(cmd, 'Initialize')
    % Child protection: Psychtoolbox userspace rendering context active as
    % required?
    [targetwindow, IsOpenGLRendering] = Screen('GetOpenGLDrawMode');
    if ~IsOpenGLRendering
        % No: We fail, but not without closing the onscreen window and
        % releasing other resources:
        sca;
        
        error('HordeHelper: Tried to call "Initialize" without calling Screen(''BeginOpenGL'',...) beforehand!');
    end
    
    % Call Initialize routine of Horde engine core to setup the engine for
    % all further commands:
    Horde3DCore('Initialize');
    
    % Eat gl errors to cope with Apples fragile OSX:
    while glGetError; end;

    % Define all relevant enums for parameter passing to engine:
    HE.H3DResTypes.Undefined  = 0;
    HE.H3DResTypes.SceneGraph = 1;
    HE.H3DResTypes.Geometry   = 2;
    HE.H3DResTypes.Animation  = 3;
    HE.H3DResTypes.Material   = 4;
    HE.H3DResTypes.Code       = 5;
    HE.H3DResTypes.Shader     = 6;
    HE.H3DResTypes.Texture    = 7;
    HE.H3DResTypes.ParticleEffect = 8;
    HE.H3DResTypes.Pipeline   = 9;

    HE.H3DRootNode = 1;

    HE.H3DOptions.MaxLogLevel         = 1;
    HE.H3DOptions.MaxNumMessages      = 2;
    HE.H3DOptions.TrilinearFiltering  = 3;
    HE.H3DOptions.MaxAnisotropy       = 4;
    HE.H3DOptions.TexCompression      = 5;
    HE.H3DOptions.LoadTextures        = 6;
    HE.H3DOptions.FastAnimation       = 7;
    HE.H3DOptions.ShadowMapSize       = 8;
    HE.H3DOptions.SampleCount         = 9;
    HE.H3DOptions.WireframeMode       = 10;
    HE.H3DOptions.DebugViewMode       = 11;
    HE.H3DOptions.DumpFailedShaders   = 12;
    
    HE.H3DNodeTypes.Undefined = 0;
    HE.H3DNodeTypes.Group     = 1;
    HE.H3DNodeTypes.Model     = 2;
    HE.H3DNodeTypes.Mesh      = 3;
    HE.H3DNodeTypes.Joint     = 4;
    HE.H3DNodeTypes.Light     = 5;
    HE.H3DNodeTypes.Camera    = 6;
    HE.H3DNodeTypes.Emitter   = 7;

    HE.H3DLight.MatResI     = 500;
    HE.H3DLight.RadiusF     = 501;
    HE.H3DLight.FovF        = 502;
    HE.H3DLight.ColorF3     = 503;
%    HE.H3DLight.Col_G       = 504;  % TODO: figure out the colors
%    HE.H3DLight.Col_B       = 505;
    HE.H3DLight.ShadowMapCountI     = 504;
    HE.H3DLight.ShadowSplitLambdaF  = 505;
    HE.H3DLight.ShadowMapBiasF      = 506;

    HE.H3DCamera.PipeResI        = 600;
    HE.H3DCamera.OutTexResI          = 601;
    HE.H3DCamera.OutBufIndexI  = 602;
    HE.H3DCamera.LeftPlaneF          = 603;
    HE.H3DCamera.RightPlaneF         = 604;
    HE.H3DCamera.BottomPlaneF        = 605;
    HE.H3DCamera.TopPlaneF           = 606;
    HE.H3DCamera.NearPlaneF          = 607;
    HE.H3DCamera.FarPlaneF           = 608;
    HE.H3DCamera.OrthoI       = 609;
    HE.H3DCamera.OccCullingI   = 610;

    HE.H3DModel.GeoResI         = 200;
    HE.H3DModel.SWSkinningI    = 201;
    HE.H3DModel.LodDist1F            = 202;
    HE.H3DModel.LodDist2F            = 203;
    HE.H3DModel.LodDist3F            = 204;
    HE.H3DModel.LodDist4F            = 205;

    % Ready to rock!
    return;
end

if strcmpi(cmd, 'GetRendertargetForWindow')
    if nargin < 2
        error('HordeHelper: GetRendertargetForWindow: Window handle missing!');
    end
    
    % Get 'win'dowhandle of target window:
    win = varargin{1};
    
    wk  = Screen('WindowKind', win);
    if ~ismember(wk, [-1, 1])
        error('Invalid windowhandle passed! Not an onscreen or offscreen window handle!');
    end
    
    if wk == 1
        % Onscreen window:
        thingo.onscreen = 1;
        % Perform dummy info query to get its drawBuffer bound:
        winfo = Screen('GetWindowInfo', win);
        
        % Onscreen with imaging pipeline enabled?
        if ~bitand(winfo.ImagingMode, kPsychNeedFastBackingStore)
            % Not a imaging pipeline backed onscreen window: No-Op. Just
            % return windowhandle back to caller:
            varargout{1} = win;
            return;
        end
        
        % Real FBO backed onscreen window with imaging pipeline active:
        
        % Query its bound FBO's color attachment texture handle:
        thingo.wofftex = glGetFramebufferAttachmentParameterivEXT(GL.FRAMEBUFFER_EXT, GL.COLOR_ATTACHMENT0_EXT, GL.FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT);
    else
        % Offscreen window: Query its texture handle directly:
        thingo.wofftex = Screen('GetOpenGLTexture', win, win);
        thingo.onscreen = 0;
    end

    % Assign remaining properties needed later for 'EndOpenGL' ops:
    thingo.win = win;
    [thingo.w thingo.h] = Screen('WindowSize', win);
    varargout{1} = thingo;

    return;
end

if strcmpi(cmd, 'EndOpenGL')
    if nargin < 2
        error('HordeHelper: EndOpenGL: Rendertarget parameter missing!');
    end

    % Get parameter:
    thingo = varargin{1};
    
    if isstruct(thingo)
        % Real rendertarget. Do the end/unbind/rebind/copy op:
        Screen('EndOpenGL', thingo.win);
    else
        % Normal window. Just do regular end op:
        Screen('EndOpenGL', thingo);
    end
    
    return;
end

error('Unknown command %s specified to HordeHelper.', cmd);

return;
