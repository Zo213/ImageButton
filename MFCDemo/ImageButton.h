#pragma once


// CImageButton

// ReSharper disable once CppClassCanBeFinal
// ReSharper disable once CppInconsistentNaming
class CImageButton : public CButton  // NOLINT(hicpp-special-member-functions, cppcoreguidelines-special-member-functions)
{
	DECLARE_DYNAMIC(CImageButton)

	CImageButton();
	virtual ~CImageButton();

protected:
	DECLARE_MESSAGE_MAP()
	// ReSharper disable once IdentifierTypo
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	void PreSubclassWindow() override;
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
public:

	/**
	 * \brief ���ð�ťͼƬ, ֧��[BMP,GIF,JPEG,PNG,TIFF]
	 * \param nNormalId ����״̬ͼƬ��Դid
	 * \param nHoverId ��ͣ״̬ͼƬ��Դid
	 * \param nPressId ����״̬ͼƬ��Դid
	 * \param nDisableId ����״̬ͼƬ��Դid, Ĭ��Ϊ0, ʹ��nNormalId
	 * \param lpszResourceType ��Դ����, Ĭ��_T("PNG")
	 */
	void SetImages(UINT nNormalId, UINT nHoverId, UINT nPressId, UINT nDisableId = 0, LPCTSTR lpszResourceType = _T("PNG"));

	/**
	 * \brief �Զ���С
	 */
	void AutoSize();
	
protected:

	/**
	 * \brief ����Դ����ͼƬ
	 * \param img ͼƬ����
	 * \param nId ͼƬ��Դid
	 * \param lpszResourceType ��Դ����
	 */
	static void LoadImageFromResource(CImage& img, UINT nId, LPCTSTR lpszResourceType);


	/**
	 * \brief �ͷ�ͼƬ��Դ
	 */
	void ReleaseImages();
	
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


