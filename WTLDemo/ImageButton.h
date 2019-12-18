#pragma once
#include <atlwin.h>
#include <atlimage.h>

// CImageButton

// ReSharper disable once CppClassCanBeFinal
// ReSharper disable once CppInconsistentNaming
class CImageButton // NOLINT(hicpp-special-member-functions, cppcoreguidelines-special-member-functions)
	: public CWindowImpl<CImageButton, CButton> 
	, public COwnerDraw<CImageButton>
{
public:
	CImageButton()
		: m_images()
		, m_bMouseTrack(TRUE)
	{
		
	}
	
	virtual ~CImageButton()
	{
		ReleaseImages();
	}

	BEGIN_MSG_MAP(CImageButton)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_MOUSEHOVER, OnMouseHover)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		CHAIN_MSG_MAP_ALT(COwnerDraw<CImageButton>, 1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()


	/**
	 * \brief ���������ֵ��dpi��ֵ
	 * \param xy ��ֵ
	 * \param dpi ��ǰdpi
	 */
#define DPI(xy, dpi) MulDiv(xy, dpi, 96)  // NOLINT(cppcoreguidelines-macro-usage)

	 /**
	  * \brief ��ȡdpi
	  * \param hWnd ���ھ��
	  * \return dpi x
	  */
	int GetDpi(HWND hWnd)
	{
		// GetDeviceCaps()֧��win2000+����ϵͳ
		const auto hDc = GetDC();
		const auto	nDpi = GetDeviceCaps(hDc, LOGPIXELSX);
		// GetDeviceCaps(hDc, LOGPIXELSY);
		ReleaseDC(hDc);
		return nDpi;
	}
	
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		CDC dc = lpDrawItemStruct->hDC;

		// ��ť�Ƿ񱻽���
		if (lpDrawItemStruct->itemState & ODS_DISABLED)
		{
			m_state = Disable;
		}

		CDC memDc;
		CBitmap bmp;

		// ƥ��ͼƬ
		auto&& image = m_images[m_state];
		if (image.IsNull())
		{
			dc.DrawText(_T("δ����ͼ��"), _tcslen(_T("δ����ͼ��")), &lpDrawItemStruct->rcItem, DT_CENTER | DT_VCENTER);
			return;
		}

		const auto nDpi = GetDpi(lpDrawItemStruct->hwndItem);

		// ͼƬ���
		const auto nWidth = DPI(image.GetWidth(), nDpi);

		// ͼƬ�߶�
		const auto nHeight = DPI(image.GetHeight(), nDpi);

		// ����˫������ڴ�dc
		memDc.CreateCompatibleDC(dc);
		// �����ڴ�dc���ݵ�λͼ
		bmp.CreateCompatibleBitmap(dc, nWidth, nHeight);
		// �ڴ�dcѡ��λͼ
		memDc.SelectBitmap(bmp);

		// ���Ƶ��ڴ�dc
		// image.StretchBlt(memDc, 0, 0, nWidth, nHeight, 0, 0, image.GetWidth(), image.GetHeight(), SRCCOPY);
		image.Draw(memDc, 0, 0, nWidth, nHeight, 0, 0, image.GetWidth(), image.GetHeight());

		// ʹ��͸��ͨ����ͼ
		BLENDFUNCTION bf;
		bf.AlphaFormat = AC_SRC_ALPHA;
		bf.BlendFlags = 0;
		bf.BlendOp = AC_SRC_OVER;
		bf.SourceConstantAlpha = 255;
		dc.AlphaBlend(0, 0, nWidth, nHeight, memDc, 0, 0, nWidth, nHeight, bf);
		
		memDc.RestoreDC(-1);
	}

protected:
	
	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once IdentifierTypo
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// ������������Ϣ, ʵ����ȫ͸��
		bHandled = TRUE;
		return 0;
	}

	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// ��갴��״̬
		m_state = Press;
		Invalidate();

		bHandled = FALSE;
		return TRUE;
	}

	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// ��굯������Ϊ��ͣ״̬, ��Ϊ��껹�ڰ�ť��
		m_state = Hover;
		Invalidate();
		bHandled = FALSE;
		return TRUE;
	}

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// ׷�����
		if (m_bMouseTrack)
		{
			TRACKMOUSEEVENT eventTrack;
			eventTrack.cbSize = sizeof(eventTrack);
			eventTrack.dwFlags = TME_LEAVE | TME_HOVER;
			eventTrack.hwndTrack = m_hWnd;					// ָ��Ҫ׷�ٵĴ���
			eventTrack.dwHoverTime = 1;						// ����ڰ�ť��ͣ������1ms, ����Ϊ״̬ΪHOVER
			_TrackMouseEvent(&eventTrack);					// ����Windows��WM_MOUSELEAVE, WM_MOUSEHOVER�¼�֧��
			m_bMouseTrack = FALSE;							// ���Ѿ�׷��, ��ֹͣ׷��
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnMouseHover(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// �����ͣ״̬
		m_state = Hover;
		Invalidate();

		bHandled = FALSE;
		return 0;
	}
	
	LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// ����뿪״̬
		m_state = Normal;

		// ����׷�����
		m_bMouseTrack = TRUE;

		Invalidate();

		bHandled = FALSE;
		return 0;
	}
		
public:

	BOOL SubclassWindow(HWND hWnd)
	{
		const auto bRet = CWindowImpl<CImageButton, CButton>::SubclassWindow(hWnd);

		if (bRet )
		{
			ModifyStyle(0, BS_OWNERDRAW);
			SetButtonStyle(GetButtonStyle() | BS_OWNERDRAW);
		}

		return bRet;
	}
	
	/**
	 * \brief ���ð�ťͼƬ, ֧��[BMP,GIF,JPEG,PNG,TIFF]
	 * \param nNormalId ����״̬ͼƬ��Դid
	 * \param nHoverId ��ͣ״̬ͼƬ��Դid
	 * \param nPressId ����״̬ͼƬ��Դid
	 * \param nDisableId ����״̬ͼƬ��Դid, Ĭ��Ϊ0, ʹ��nNormalId
	 * \param lpszResourceType ��Դ����, Ĭ��_T("PNG")
	 */
	void SetImages(UINT nNormalId, UINT nHoverId, UINT nPressId, UINT nDisableId = 0, LPCTSTR lpszResourceType = _T("PNG"))
	{
		ReleaseImages();
		LoadImageFromResource(m_images[Normal], nNormalId, lpszResourceType);
		LoadImageFromResource(m_images[Hover], nHoverId, lpszResourceType);
		LoadImageFromResource(m_images[Press], nPressId, lpszResourceType);
		nDisableId = nDisableId == 0 ? nNormalId : nDisableId;
		LoadImageFromResource(m_images[Disable], nDisableId, lpszResourceType);
		// AutoSize();

	}

	/**
	 * \brief �Զ���С
	 */
	void AutoSize()
	{
		auto&& image = m_images[Normal];
		if (image.IsNull())
		{
			return;
		}

		const auto nDpi = GetDpi(m_hWnd);

		SetWindowPos(nullptr, -1, -1, DPI(image.GetWidth(), nDpi), DPI(image.GetHeight(), nDpi), SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);
	}
	
protected:

	/**
	 * \brief ����Դ����ͼƬ
	 * \param img ͼƬ����
	 * \param nId ͼƬ��Դid
	 * \param lpszResourceType ��Դ����
	 */
	static void LoadImageFromResource(CImage& img, UINT nId, LPCTSTR lpszResourceType)
	{
		const static auto hInstance = ModuleHelper::GetResourceInstance(); // ModuleHelper::GetModuleInstance();

		// ������Դ
		const auto hResource = ::FindResource(hInstance, MAKEINTRESOURCE(nId), lpszResourceType);
		if (hResource == nullptr)
		{
			return;
		}

		// ������Դ
		const auto hResData = LoadResource(hInstance, hResource);
		if (hResData == nullptr)
		{
			return;
		}

		// ������Դ���õ���Դ����ָ��
		const auto lpResource = LockResource(hResData);
		if (lpResource == nullptr)
		{
			return;
		}

		// ��Դ��С
		const auto dwResourceSize = SizeofResource(hInstance, hResource);

		// Ϊ��������Դ
		const auto hMem = GlobalAlloc(GMEM_FIXED, dwResourceSize);
		if (hMem == nullptr)
		{
			return;
		}

		const auto lpMem = static_cast<LPBYTE>(GlobalLock(hMem));
		if (lpMem == nullptr)
		{
			GlobalFree(hMem);
			return;
		}

		// ������Դ���ݵ���
		memcpy(lpMem, lpResource, dwResourceSize);

		// ���ڴ洴����
		const auto pStream = SHCreateMemStream(lpMem, dwResourceSize);
		if (pStream != nullptr)
		{
			if (img.Load(pStream) == S_OK && _tcsicmp(lpszResourceType, _T("PNG")) == 0 && img.GetBPP() == 32)
			{
				// Ԥ����PNG��Alphaͨ��
				for (auto i = 0; i < img.GetWidth(); i++)
				{
					for (auto j = 0; j < img.GetHeight(); j++)
					{
						const auto pColor = reinterpret_cast<LPBYTE>(img.GetPixelAddress(i, j));
						pColor[0] = pColor[0] * pColor[3] / 255;
						pColor[1] = pColor[1] * pColor[3] / 255;
						pColor[2] = pColor[2] * pColor[3] / 255;
					}
				}
			}
		}

		GlobalUnlock(lpMem);
		GlobalFree(hMem);
	}


	/**
	 * \brief �ͷ�ͼƬ��Դ
	 */
	void ReleaseImages()
	{
		for (auto&& image : m_images)
		{
			image.Destroy();
		}
	}
	
	/**
	 * \brief ��ť״̬
	 */
	enum State
	{
		/**
		 * \brief ����״̬
		 */
		Normal = 0,

		/**
		 * \brief ��ͣ״̬
		 */
		Hover,

		/**
		 * \brief ����״̬
		 */
		Press,

		/**
		 * \brief ����״̬
		 */
		Disable,
	};
	

	/**
	 * \brief 4�ְ�ť״̬ͼƬ
	 */
	CImage		m_images[4];

	/**
	 * \brief ��ť״̬
	 */
	State		m_state = Normal;
	
	/**
	 * \brief ���׷��
	 */
	BOOL        m_bMouseTrack;
};


