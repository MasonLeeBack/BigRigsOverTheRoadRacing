#include <r3dPCH.h>
#include <r3d.h>

int r3dRenderLayer::IsTextureFormatAvailable(D3DFORMAT Fmt)
{
	HRESULT	res;

	res = pd3d->CheckDeviceFormat(
		D3DADAPTER_DEFAULT,
		DeviceType,
		d3dpp.BackBufferFormat,
		0,
		D3DRTYPE_TEXTURE,
		Fmt);

	return res == D3D_OK;
}

r3dRenderLayer::r3dRenderLayer()
{
}

r3dRenderLayer::~r3dRenderLayer()
{
}

struct
{
	const char* ModeName;
	int		ModeCode;
} \
_ModeNames[] =
{
	{ "D3DFMT_R8G8B8               ", 20 },
	{ "D3DFMT_A8R8G8B8             ", 21 },
	{ "D3DFMT_X8R8G8B8             ", 22 },
	{ "D3DFMT_R5G6B5               ", 23 },
	{ "D3DFMT_X1R5G5B5             ", 24 },
	{ "D3DFMT_A1R5G5B5             ", 25 },
	{ "D3DFMT_A4R4G4B4             ", 26 },
	{ "D3DFMT_R3G3B2               ", 27 },
	{ "D3DFMT_A8                   ", 28 },
	{ "D3DFMT_A8R3G3B2             ", 29 },
	{ "D3DFMT_X4R4G4B4             ", 30 },
	{ "D3DFMT_A8P8                 ", 40 },
	{ "D3DFMT_P8                   ", 41 },
	{ "D3DFMT_L8                   ", 50 },
	{ "D3DFMT_A8L8                 ", 51 },
	{ "D3DFMT_A4L4                 ", 52 },
	{ "D3DFMT_V8U8                 ", 60 },
	{ "D3DFMT_L6V5U5               ", 61 },
	{ "D3DFMT_X8L8V8U8             ", 62 },
	{ "D3DFMT_Q8W8V8U8             ", 63 },
	{ "D3DFMT_V16U16               ", 64 },
	{ "D3DFMT_W11V11U10            ", 65 },
	{ "D3DFMT_D16_LOCKABLE         ", 70 },
	{ "D3DFMT_D32                  ", 71 },
	{ "D3DFMT_D15S1                ", 73 },
	{ "D3DFMT_D24S8                ", 75 },
	{ "D3DFMT_D16                  ", 80 },
	{ "D3DFMT_D24X8                ", 77 },
	{ "D3DFMT_D24X4S4              ", 79 },
	{ "D3DFMT_VERTEXDATA           ", 100 },
	{ "D3DFMT_INDEX16              ", 101 },
	{ "D3DFMT_INDEX32              ", 102 },
};
static	int	_Num_ModeNames = sizeof(_ModeNames) / sizeof(*_ModeNames);

const char* __GetFormatName(int ModeCode)
{
	for (int i = 0; i < _Num_ModeNames; i++)
		if (_ModeNames[i].ModeCode == ModeCode) return _ModeNames[i].ModeName;

	return "UNDEFINED MODE";
}

void __EvaluateDisplayModes(IDirect3D9* d3d)
{
	// search if that mode available..
	D3DDISPLAYMODE mode;
	D3DFORMAT displayFormats[] =
	{
		D3DFMT_X8R8G8B8 // todo: are there other display formats we should count? (old r3d)
	};

	for (int k = 0; k < _countof(displayFormats); ++k)
	{
		D3DFORMAT fmt = displayFormats[k];
		int n_modes = d3d->GetAdapterModeCount(D3DADAPTER_DEFAULT, fmt);

		for (int i = 0; i < n_modes; i++)
		{
			d3d->EnumAdapterModes(D3DADAPTER_DEFAULT, fmt, i, &mode);
			r3dOutToLog ("Mode %d:  \t  %dx%d  \tFormat: %s\n", i, (int)mode.Width, (int)mode.Height, __GetFormatName((int)mode.Format));
		}
	}
}

HANDLE r3d_Thread1;

int r3dRenderLayer::Init(HWND hWindow, const char* displayName)
{
    extern	void 		r3d_UtilityThread(DWORD in);

    // expose this process handle to other threads
    DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(),
        &r3d_MainThread, THREAD_ALL_ACCESS, TRUE, 0);

	// create utility thread
	DWORD temp;
	r3d_Thread1 = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)r3d_UtilityThread,
			(void*)NULL,		// argument
			0,					// creation flags
			&temp				// thread ID
			);

	Version = R3D_ETERNITY_VERSION;
	HLibWin = hWindow;

	pdi = NULL;
	pd3d = NULL;

    r3dInitLogFile(); // bad coding practice, why are we initializing this in the render layer...

	//Todo: initialize mouse+kb classes... still, terrible coding practice...
	// Keyboard = game_new r3dKeyboard;
	// Mouse = game_new r3dMouse;

	if (DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pdi, NULL) == DI_OK)
	{
		// Keyboard->SetCapture();
	}
	r3dOutToLog("Input Devices enabled: Mouse, Keyboard\n");

	pd3d = Direct3DCreate9(D3D_SDK_VERSION);
	
	if (pd3d == NULL)
		return FALSE;

    return TRUE;
}

int ToBeDetermined = 0;

//
// The SetMode implementation *should* be complete, however, we should
// figure out the flags soon.
//
int r3dRenderLayer::SetMode(int xRes, int yRes, int BPP, int flags)
{
	HRESULT res;
	BOOL isWindowed = 1;
	D3DDISPLAYMODE displayMode;

	displayMode.Format = (flags & 8) != 0 ? D3DFMT_D24S8 : D3DFMT_D16;
	isWindowed = (flags & 4) >> 2;
	if (flags & 32) {
		DeviceType = D3DDEVTYPE_REF;
	}
	else {
		DeviceType = D3DDEVTYPE_HAL;
	}
	CurrentBPP = BPP;
	__EvaluateDisplayModes(pd3d);

	// Check to see if the requested mode is a valid mode on the renderer
	D3DFORMAT DisplayFormat = CurrentBPP > 16 ? DISPLAY_FORMAT_32 : DISPLAY_FORMAT_16;
	for (UINT i = 0, e = pd3d->GetAdapterModeCount(D3DADAPTER_DEFAULT, DisplayFormat); i < 3; i++) {
		D3DDISPLAYMODE mode;
		HRESULT hr = pd3d->EnumAdapterModes(D3DADAPTER_DEFAULT, DisplayFormat, i, &mode);

		if (hr == D3D_OK)
		{
			displayMode.Width = mode.Width;
			displayMode.Height = mode.Height;
			displayMode.Format = mode.Format;
			if (mode.Width == xRes && mode.Height == yRes)
			{
				if (mode.Format == DISPLAY_FORMAT_16)
					break;
			}
			if (mode.Width != xRes || mode.Height != yRes)
			{
				if (mode.Format == DisplayFormat)
					break;
			}
		}
	}

	// Set up the structure used to create the D3DDevice. Since we are now
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.hDeviceWindow = HLibWin;
	d3dpp.BackBufferWidth = displayMode.Width;
	d3dpp.BackBufferHeight = displayMode.Height;
	d3dpp.BackBufferFormat = displayMode.Format;
	d3dpp.Windowed = isWindowed;
	d3dpp.BackBufferCount = 1;
	d3dpp.AutoDepthStencilFormat = (flags & 8) != 0 ? D3DFMT_D24S8 : D3DFMT_D16;; // WHY isn't this working?
	d3dpp.EnableAutoDepthStencil = TRUE; // WHY isn't this working?
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
	d3dpp.Flags = NULL;

	if (!isWindowed)
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Figure out the device capabilities. (For vertex shader versions, we should make a define to easily create these, (vers 0.0, 1.0, 1.1)
	D3DCAPS9 deviceCapabilities;
	ZeroMemory(&deviceCapabilities, sizeof(D3DCAPS9));
	pd3d->GetDeviceCaps(D3DADAPTER_DEFAULT, DeviceType, &deviceCapabilities);
	if (deviceCapabilities.VertexShaderVersion == 0xFFFE0101)
	{
		ToBeDetermined = 64;
		r3dOutToLog("Creating Hardware Vertex Shaders\n");
	}
	else
	{
		ToBeDetermined = 32;
		r3dOutToLog("Creating Software Vertex Shaders\n");
	}
	r3dOutToLog("Version is %d. Asked for %d and %d and %d\n", deviceCapabilities.VertexShaderVersion, 0xFFFE0000, 0xFFFE0100, 0xFFFE0101);
	ToBeDetermined = 128;

	res = pd3d->CreateDevice(D3DADAPTER_DEFAULT, DeviceType, HLibWin, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pd3ddev);
	if (res == D3DERR_INVALIDCALL) r3dOutToLog("Error: Invalid Call\n");
	if (res == D3DERR_NOTAVAILABLE) r3dOutToLog("Error: Not Available\n");
	if (res == D3DERR_OUTOFVIDEOMEMORY) r3dOutToLog("Error: Out of video memory\n");

	if (res != D3D_OK)
	{
		res = pd3d->CreateDevice(D3DADAPTER_DEFAULT, DeviceType, HLibWin, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pd3ddev);
		if (res != D3D_OK)
		{
			return FALSE;
		}
	}


	// some call i can't be bothered to reverse atm
	// sub_416E80();

	// Texture format's listing
	r3dOutToLog ("Texture Formats availability\n");
	for(int i=0; i<_Num_ModeNames; i++)
	{
		if ( IsTextureFormatAvailable((D3DFORMAT)_ModeNames[i].ModeCode))
			r3dOutToLog ("Texture Format found ID %d: \t %s\n", _ModeNames[i].ModeCode,_ModeNames[i].ModeName);
	}

	r3dOutToLog ("Special Texture Formats availability\n");
	if ( IsTextureFormatAvailable(D3DFMT_UYVY))
		r3dOutToLog ("Special Texture Format found:\t D3DFMT_UYVY\n");
	if ( IsTextureFormatAvailable(D3DFMT_YUY2))
		r3dOutToLog ("Special Texture Format found:\t D3DFMT_YUY2\n");
	if ( IsTextureFormatAvailable(D3DFMT_DXT1))
		r3dOutToLog ("Special Texture Format found:\t D3DFMT_DXT1\n");
	if ( IsTextureFormatAvailable(D3DFMT_DXT2))
		r3dOutToLog ("Special Texture Format found:\t D3DFMT_DXT2\n");
	if ( IsTextureFormatAvailable(D3DFMT_DXT3))
		r3dOutToLog ("Special Texture Format found:\t D3DFMT_DXT3\n");
	if ( IsTextureFormatAvailable(D3DFMT_DXT4))
		r3dOutToLog ("Special Texture Format found:\t D3DFMT_DXT4\n");
	if ( IsTextureFormatAvailable(D3DFMT_DXT5))
		r3dOutToLog ("Special Texture Format found:\t D3DFMT_DXT5\n");

    return TRUE;
}
