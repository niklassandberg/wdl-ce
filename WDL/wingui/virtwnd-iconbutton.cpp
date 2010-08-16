/*
    WDL - virtwnd-iconbutton.cpp
    Copyright (C) 2006 and later Cockos Incorporated

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
      

    Implementation for virtual window icon buttons, icons, combo boxes, and static text controls.

*/

#include "virtwnd-controls.h"
#include "../lice/lice.h"

WDL_VirtualIconButton::WDL_VirtualIconButton()
{
  m_checkstate=-1;
  m_textfont=0;
  m_textalign=0;
  m_bgcol1_msg=0;
  m_is_button=true;
  m_pressed=0;
  m_iconCfg=0;
  m_en=true;
  m_forceborder=false;
}

WDL_VirtualIconButton::~WDL_VirtualIconButton()
{
}

void WDL_VirtualIconButton::SetTextLabel(const char *text, char align, HFONT font) 
{ 
  if (font) m_textfont=font;
  m_textalign=align;
  m_textlbl.Set(text); 
  if (!m_iconCfg) RequestRedraw(NULL); 
} 

void WDL_VirtualIconButton::SetCheckState(char state)
{
  if (state != m_checkstate)
  {
    m_checkstate=state;
    if (!m_iconCfg) RequestRedraw(NULL);
  }
}


void WDL_VirtualIconButton::OnPaintOver(LICE_SysBitmap *drawbm, int origin_x, int origin_y, RECT *cliprect)
{
  if (m_iconCfg && m_iconCfg->olimage)
  {
    RECT r;
    GetPositionPaintOverExtent(&r);

    int sx=0;
    int sy=0;
    int w=m_iconCfg->olimage->getWidth();
    int h=m_iconCfg->olimage->getHeight();
    if (m_iconCfg->image_ltrb_used)  { w-=2; h-= 2; sx++,sy++; }

    w/=3;

    if (w>0 && h>0)
    {
      if (m_is_button)
      {
        if ((m_pressed&2))  sx+=(m_pressed&1) ? w*2 : w;
      }
      LICE_ScaledBlit(drawbm,m_iconCfg->olimage,r.left+origin_x,r.top+origin_y,
        r.right-r.left,
        r.bottom-r.top,
        (float)sx,(float)sy,(float)w,(float)h,1.0f,
        LICE_BLIT_MODE_COPY|LICE_BLIT_FILTER_BILINEAR|LICE_BLIT_USE_ALPHA);      
    }
  }
}


void WDL_VirtualIconButton::OnPaint(LICE_SysBitmap *drawbm, int origin_x, int origin_y, RECT *cliprect) 
{ 
  HDC hdc=drawbm->getDC();
  int col;

  if (m_iconCfg && m_iconCfg->image)
  {
    RECT r=m_position;

    int sx=0;
    int sy=0;
    int w=m_iconCfg->image->getWidth();
    int h=m_iconCfg->image->getHeight();

    w/=3;
    if (w>0 && h > 0)
    {
      if (m_is_button)
      {
        if ((m_pressed&2))  sx+=(m_pressed&1) ? w*2 : w;
      }

      LICE_ScaledBlit(drawbm,m_iconCfg->image,r.left+origin_x,r.top+origin_y,
        r.right-r.left,
        r.bottom-r.top,
        (float)sx,(float)sy,(float)w,(float)h,1.0f,
        LICE_BLIT_MODE_COPY|LICE_BLIT_FILTER_BILINEAR|LICE_BLIT_USE_ALPHA);      
    }
  }
  else
  {
    RECT r=m_position;
    r.left+=origin_x;
    r.right+=origin_x;
    r.top+=origin_y;
    r.bottom+=origin_y;
    if (m_is_button)
    {
      if (WDL_STYLE_WantGlobalButtonBackground(&col))
      {
        HBRUSH br=CreateSolidBrush(col);
        FillRect(hdc,&r,br);
        DeleteObject(br);
      }

      if ((m_pressed&2) || m_forceborder || WDL_STYLE_WantGlobalButtonBorders())
      {
        int cidx=(m_pressed&1)?COLOR_3DSHADOW:COLOR_3DHILIGHT;

        HPEN pen=CreatePen(PS_SOLID,0,WDL_STYLE_GetSysColor(cidx));
        HGDIOBJ open=SelectObject(hdc,pen);

        MoveToEx(hdc,r.left,r.bottom-1,NULL);
        LineTo(hdc,r.left,r.top);
        LineTo(hdc,r.right-1,r.top);
        SelectObject(hdc,open);
        DeleteObject(pen);
        cidx=(m_pressed&1)?COLOR_3DHILIGHT:COLOR_3DSHADOW;
        pen=CreatePen(PS_SOLID,0,WDL_STYLE_GetSysColor(cidx));
        open=SelectObject(hdc,pen);

        LineTo(hdc,r.right-1,r.bottom-1);
        LineTo(hdc,r.left,r.bottom-1);
        SelectObject(hdc,open);
        DeleteObject(pen);

      }
    }
    if (m_iconCfg && m_iconCfg->hIcon)
    {
      int sz=16,sz2=16;
      WDL_STYLE_ScaleImageCoords(&sz,&sz2);

      //if (m_position.right-m_position.left > 24) sz=m_position.right-m_position.left-8;
    
      int x=r.left+((r.right-r.left)-sz)/2;
      int y=r.top+((r.bottom-r.top)-sz2)/2;
      if (m_is_button)
      {
        if ((m_pressed&3)==3) { x++; y++; }
      }
  #ifdef _WIN32
      DrawIconEx(hdc,x,y,m_iconCfg->hIcon,sz,sz2,0,NULL,DI_NORMAL);
  #else
      RECT r={x,y,x+sz,y+sz2};
      DrawImageInRect(hdc,m_iconCfg->hIcon,&r);
  #endif
    }
    if (!m_iconCfg && m_textlbl.Get()[0])
    {
      RECT r2=r;
      if (m_checkstate>=0)
      {
        RECT tr=r2;
        int sz=tr.bottom-tr.top;
        r2.left+=sz+2;

        tr.top+=2;
        tr.bottom-=2;
        sz-=4;
        sz&=~1;
        LICE_FillRect(drawbm ,tr.left,tr.top,sz,sz,LICE_RGBA(255,255,255,255),1.0f,LICE_BLIT_MODE_COPY);
        LICE_Line(drawbm,tr.left,tr.top,tr.left+sz,tr.top,LICE_RGBA(128,128,128,255),1.0f,LICE_BLIT_MODE_COPY,false);
        LICE_Line(drawbm,tr.left+sz,tr.top,tr.left+sz,tr.bottom,LICE_RGBA(128,128,128,255),1.0f,LICE_BLIT_MODE_COPY,false);
        LICE_Line(drawbm,tr.left+sz,tr.bottom,tr.left,tr.bottom,LICE_RGBA(128,128,128,255),1.0f,LICE_BLIT_MODE_COPY,false);
        LICE_Line(drawbm,tr.left,tr.bottom,tr.left,tr.top,LICE_RGBA(128,128,128,255),1.0f,LICE_BLIT_MODE_COPY,false);
        int nl = (m_checkstate>0) ? 3:0;        
        if (m_pressed&1) nl ^= 2;

        if (nl&1)
          LICE_Line(drawbm,tr.left+2,tr.bottom-2,tr.left+sz-2,tr.top+2,LICE_RGBA(0,0,0,255),1.0f,LICE_BLIT_MODE_COPY,false);
        if (nl&2)
          LICE_Line(drawbm,tr.left+2,tr.top+2,tr.left+sz-2,tr.bottom-2,LICE_RGBA(0,0,0,255),1.0f,LICE_BLIT_MODE_COPY,false);


      }
      // draw text
      SetBkMode(hdc,TRANSPARENT);
      SetTextColor(hdc,WDL_STYLE_GetSysColor(COLOR_BTNTEXT));
      HGDIOBJ of=NULL;
      if (m_textfont) of=SelectObject(hdc,m_textfont);
      if (m_pressed&1)
      {
        if (m_textalign<0) r2.left+=1;
        else if (m_textalign>0) r2.right+=1;
        else r2.left+=2;
        r2.top+=2;
      }
      DrawText(hdc,m_textlbl.Get(),-1,&r2,DT_SINGLELINE|DT_VCENTER|(m_textalign<0?DT_LEFT:m_textalign>0?DT_RIGHT:DT_CENTER)|DT_NOPREFIX);
      if (m_textfont) SelectObject(hdc,of);
      
    }
  }

  if (m_bgcol1_msg)
  {
    int brcol=-100;
    SendCommand(m_bgcol1_msg,(INT_PTR)&brcol,GetID(),this);
    if (brcol != -100)
    {
      RECT r=m_position;

      int bh=(r.bottom-r.top)/5;
      if (bh<1) bh=1;
      int bw=(r.right-r.left)/5;
      if (bw<1) bw=1;

      LICE_FillRect(drawbm,
        r.left+origin_x,r.top+origin_y,
        r.right-r.left,
        bh,LICE_RGBA_FROMNATIVE(brcol,255),0.75,LICE_BLIT_MODE_COPY);

      LICE_FillRect(drawbm,
        r.left+origin_x,r.top+origin_y+bh,
        bw,
        r.bottom-r.top-bh*2,LICE_RGBA_FROMNATIVE(brcol,255),0.75,LICE_BLIT_MODE_COPY);

      LICE_FillRect(drawbm,
        r.right+origin_x-bw,r.top+origin_y+bh,
        bw,
        r.bottom-r.top-bh*2,LICE_RGBA_FROMNATIVE(brcol,255),0.75,LICE_BLIT_MODE_COPY);

      LICE_FillRect(drawbm,
        r.left+origin_x,r.bottom+origin_y-bh,
        r.right-r.left,
        bh,LICE_RGBA_FROMNATIVE(brcol,255),0.75,LICE_BLIT_MODE_COPY);

    
    }
  }

} 


void WDL_VirtualIconButton::OnMouseMove(int xpos, int ypos)
{
  if (m_en&&m_is_button)
  {
    int wp=m_pressed;
    if (xpos >= 0&& xpos < m_position.right-m_position.left && ypos >= 0 && ypos < m_position.bottom-m_position.top)
      m_pressed|=2;
    else m_pressed&=~2;

    if ((m_pressed&3)!=(wp&3))
      RequestRedraw(NULL);
  }
}

int WDL_VirtualIconButton::OnMouseDown(int xpos, int ypos)
{
  if (m_en&&m_is_button)
  {
    m_pressed=3;
    RequestRedraw(NULL);
    return 1;
  }
  return 0;
}

bool WDL_VirtualIconButton::OnMouseDblClick(int xpos, int ypos)
{
  if (!m_is_button) return false;
  if (m_en)
  {
    int code=GetID();
    if (!m_iconCfg && m_textlbl.Get()[0] && m_checkstate >= 0)
    {
      if (xpos<(m_position.bottom-m_position.top))
        code|=600<<16;
    }
    SendCommand(WM_COMMAND,code,0,this);
  }
  return true;
}

void WDL_VirtualIconButton::OnMouseUp(int xpos, int ypos)
{
  if (!m_is_button) return;

  int waspress=!!m_pressed;
  m_pressed&=~1;
  RequestRedraw(NULL);
  if (waspress&&xpos >= 0&& xpos < m_position.right-m_position.left && ypos >= 0 && ypos < m_position.bottom-m_position.top)
  {
    if (m_en)
    {
      int code=GetID();
      if (!m_iconCfg && m_textlbl.Get()[0] && m_checkstate >= 0)
      {
        if (xpos<(m_position.bottom-m_position.top))
          code|=600<<16;
      }
      SendCommand(WM_COMMAND,code,0,this);
    }
  }
}



WDL_VirtualComboBox::WDL_VirtualComboBox()
{
  m_font=0;
  m_align=-1;
  m_curitem=-1;
}

WDL_VirtualComboBox::~WDL_VirtualComboBox()
{
  m_items.Empty(true,free);
}


int WDL_VirtualComboBox::OnMouseDown(int xpos, int ypos)
{
  if (m_items.GetSize())
  {
    HMENU menu=CreatePopupMenu();
    int x;
    for (x = 0 ; x < m_items.GetSize(); x ++)
    {
      MENUITEMINFO mi={sizeof(mi),MIIM_ID|MIIM_STATE|MIIM_TYPE,MFT_STRING,
        0,1000+x,NULL,NULL,NULL,0};
      mi.dwTypeData = (char *)m_items.Get(x);
      if (!strcmp(mi.dwTypeData,"<SEP>"))
        mi.fType=MFT_SEPARATOR;
      mi.fState = m_curitem == x ?MFS_CHECKED:0;
      InsertMenuItem(menu,x,TRUE,&mi);
    }
    HWND h=GetRealParent();
    POINT p={0,};
    WDL_VirtualWnd *w=this;
    while (w)
    {
      RECT r;
      w->GetPosition(&r);
      p.x+=r.left;
      p.y+=w==this?r.bottom:r.top;
      w=w->GetParent();
    }
    if (h) 
    {
      ClientToScreen(h,&p);
      //SetFocus(h);
    }
    int ret=TrackPopupMenu(menu,TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RETURNCMD|TPM_NONOTIFY,p.x,p.y,0,h,NULL);

    if (ret>=1000)
    {
      m_curitem=ret-1000;
      RequestRedraw(NULL);
    // track menu
      SendCommand(WM_COMMAND,GetID() | (CBN_SELCHANGE<<16),0,this);
    }
  }
  return -1;
}

void WDL_VirtualComboBox::OnPaint(LICE_SysBitmap *drawbm, int origin_x, int origin_y, RECT *cliprect)
{
  HDC hdc=drawbm->getDC();
  {
    SetBkMode(hdc,TRANSPARENT);
    HGDIOBJ of=0;
    if (m_font&&*m_font) of=SelectObject(hdc,*m_font);

    RECT r=m_position;
    r.left+=origin_x;
    r.right+=origin_x;
    r.top+=origin_y;
    r.bottom+=origin_y;

    int col=WDL_STYLE_GetSysColor(COLOR_WINDOW);
    HBRUSH br=CreateSolidBrush(col);
    FillRect(hdc,&r,br);
    DeleteObject(br);

    {
      RECT tr=r;
      tr.left=tr.right-(tr.bottom-tr.top);
      br=CreateSolidBrush(WDL_STYLE_GetSysColor(COLOR_BTNFACE));
      FillRect(hdc,&tr,br);
      DeleteObject(br);
    }
    

    int tcol=WDL_STYLE_GetSysColor(COLOR_BTNTEXT);
    int shad=WDL_STYLE_GetSysColor(COLOR_3DSHADOW);
    if (!WDL_Style_WantTextShadows(&shad)) shad=-1;
    if (m_items.Get(m_curitem)&&m_items.Get(m_curitem)[0])
    {
      RECT tr=r;
      tr.left+=2;
      tr.right-=16;
      if (shad!=-1)
      {
        SetTextColor(hdc,shad);
        RECT tr2=tr;
        tr2.top+=2;
        if (m_align==0) tr2.left+=2;
        else if (m_align>0) tr2.left+=2;
        else tr2.right--;
        DrawText(hdc,m_items.Get(m_curitem),-1,&tr2,DT_SINGLELINE|DT_VCENTER|(m_align<0?DT_LEFT:m_align>0?DT_RIGHT:DT_CENTER)|DT_NOPREFIX);
      }
      SetTextColor(hdc,tcol);
      DrawText(hdc,m_items.Get(m_curitem),-1,&tr,DT_SINGLELINE|DT_VCENTER|(m_align<0?DT_LEFT:m_align>0?DT_RIGHT:DT_CENTER)|DT_NOPREFIX);
    }


    if (of) SelectObject(hdc,of);

    HPEN pen=CreatePen(PS_SOLID,0,WDL_STYLE_GetSysColor(COLOR_3DSHADOW));
    HPEN pen3=CreatePen(PS_SOLID,2,tcol);
    HPEN pen2=CreatePen(PS_SOLID,0,WDL_STYLE_GetSysColor(COLOR_3DHILIGHT));
    of=SelectObject(hdc,pen);

    // draw the down arrow button
    {
      int bs=(r.bottom-r.top);
      int l=r.right-bs;

      int a=(bs/4)&~1;

      MoveToEx(hdc,l,r.top,NULL);
      LineTo(hdc,l,r.bottom);

      SelectObject(hdc,pen2);
      MoveToEx(hdc,l-1,r.top,NULL);
      LineTo(hdc,l-1,r.bottom);

      SelectObject(hdc,pen3);
      MoveToEx(hdc,l+bs/2 - a,r.top+bs/2 - a/2,NULL);
        LineTo(hdc,l+bs/2, r.top+bs/2+a/2);
        LineTo(hdc,l+bs/2 + a,r.top+bs/2 - a/2);

      SelectObject(hdc,pen);


    }

   

    // draw the border
    MoveToEx(hdc,r.left,r.bottom-1,NULL);
    LineTo(hdc,r.left,r.top);
    LineTo(hdc,r.right-1,r.top);
    SelectObject(hdc,pen2);
    LineTo(hdc,r.right-1,r.bottom-1);
    LineTo(hdc,r.left,r.bottom-1);

    SelectObject(hdc,of);
    DeleteObject(pen);
    DeleteObject(pen2);
    DeleteObject(pen3);


  }
}



WDL_VirtualStaticText::WDL_VirtualStaticText()
{
  m_dotint=false;
  m_bkbm=0;
  m_margin_r=m_margin_l=0;
  m_fg=m_bg=-1;
  m_wantborder=false;
  m_font=0;
  m_align=-1;
  m_wantsingle=false;
}

WDL_VirtualStaticText::~WDL_VirtualStaticText()
{
}

void WDL_VirtualStaticText::SetText(const char *text) 
{ 
  if (strcmp(m_text.Get(),text))
  {
    m_text.Set(text); 
    if (m_font) RequestRedraw(NULL); 
  }
}

int WDL_VirtualStaticText::OnMouseDown(int xpos, int ypos)
{
  if (m_wantsingle)
  {
    SendCommand(WM_COMMAND,GetID() | (STN_CLICKED<<16),0,this);
    return -1;
  }
  return 0;
}

void WDL_VirtualStaticText::OnPaint(LICE_SysBitmap *drawbm, int origin_x, int origin_y, RECT *cliprect)
{
  HDC hdc=drawbm->getDC();
  RECT r=m_position;
  r.left+=origin_x;
  r.right+=origin_x;
  r.top += origin_y;
  r.bottom += origin_y;

  if (m_bkbm && m_bkbm->bgimage)
  {
    WDL_VirtualWnd_ScaledBlitBG(drawbm,m_bkbm,
      r.left,r.top,r.right-r.left,r.bottom-r.top,
      r.left,r.top,r.right-r.left,r.bottom-r.top,
      1.0,LICE_BLIT_MODE_COPY|LICE_BLIT_FILTER_BILINEAR|LICE_BLIT_USE_ALPHA);

    if (m_dotint && m_bg>=0) 
    {
        float rv=GetRValue(m_bg)/255.0;
        float gv=GetGValue(m_bg)/255.0;
        float bv=GetBValue(m_bg)/255.0;

        float avg=(rv+gv+bv)*0.33333f;
        if (avg<0.05)avg=0.05;

        float sc=0.5f;
        float sc2 = (1.0f-sc)/avg;

        float sc3=32.0f;
        float sc4=64.0f*(avg-0.5);
        // tint
        LICE_MultiplyAddRect(drawbm,
          r.left,r.top,
            r.right-r.left,
            r.bottom-r.top,
            sc+rv*sc2,sc+gv*sc2,sc+bv*sc2,1,
            (rv-avg)*sc3+sc4,
            (gv-avg)*sc3+sc4,
            (bv-avg)*sc3+sc4,
            0);
    }
  }
  else if (m_bg!=-1)
  {
    HBRUSH h=CreateSolidBrush(m_bg);
    FillRect(hdc,&r,h);
    DeleteObject(h);
  }

  if (m_wantborder)
  {    
    int cidx=COLOR_3DSHADOW;

    HPEN pen=CreatePen(PS_SOLID,0,WDL_STYLE_GetSysColor(cidx));
    HGDIOBJ open=SelectObject(hdc,pen);

    MoveToEx(hdc,r.left,r.bottom-1,NULL);
    LineTo(hdc,r.left,r.top);
    LineTo(hdc,r.right-1,r.top);
    SelectObject(hdc,open);
    DeleteObject(pen);
    cidx=COLOR_3DHILIGHT;
    pen=CreatePen(PS_SOLID,0,WDL_STYLE_GetSysColor(cidx));
    open=SelectObject(hdc,pen);

    LineTo(hdc,r.right-1,r.bottom-1);
    LineTo(hdc,r.left,r.bottom-1);
    SelectObject(hdc,open);
    DeleteObject(pen);
    r.left++;
    r.bottom--;
    r.top++;
    r.right--;

  }

  if (m_text.Get()[0])
  {

    r.left+=m_margin_l;
    r.right-=m_margin_r;
    SetBkMode(hdc,TRANSPARENT);
    HGDIOBJ of=0;
    if (m_font&&*m_font) of=SelectObject(hdc,*m_font);
    
    int align=m_align;
#ifdef _WIN32
    if (align==0)
    {
      RECT r2={0,0,0,0};
      DrawText(hdc,m_text.Get(),-1,&r2,DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_NOPREFIX|DT_CALCRECT);
      if (r2.right > r.right-r.left) align=-1;
    }
#endif

    int tcol=m_fg!=-1 ? m_fg : WDL_STYLE_GetSysColor(COLOR_BTNTEXT);
    int shad=WDL_STYLE_GetSysColor(COLOR_3DSHADOW);
    if (WDL_Style_WantTextShadows(&shad))
    {
      SetTextColor(hdc,shad);
      RECT r2=r;
      r2.top++;
      if (align==0) r2.left+=2;
      else if (align<0) r2.left++;
      else if (align>0) r.right--;
      DrawText(hdc,m_text.Get(),-1,&r2,DT_SINGLELINE|DT_VCENTER|(align<0?DT_LEFT:align>0?DT_RIGHT:DT_CENTER)|DT_NOPREFIX);
    }
    SetTextColor(hdc,tcol);
    DrawText(hdc,m_text.Get(),-1,&r,DT_SINGLELINE|DT_VCENTER|(align<0?DT_LEFT:align>0?DT_RIGHT:DT_CENTER)|DT_NOPREFIX);

    if (of) SelectObject(hdc,of);


  }
}


bool WDL_VirtualStaticText::OnMouseDblClick(int xpos, int ypos)
{
  SendCommand(WM_COMMAND,GetID() | (STN_DBLCLK<<16),0,this);
  return true;

}


bool WDL_VirtualIconButton::WantsPaintOver()
{
  return m_is_button && m_iconCfg && m_iconCfg->image && m_iconCfg->olimage;
}

void WDL_VirtualIconButton::GetPositionPaintOverExtent(RECT *r)
{
  *r=m_position;
  if (m_iconCfg && m_iconCfg->image && m_iconCfg->olimage && m_iconCfg->image_ltrb_used)
  {
    int w=(m_iconCfg->olimage->getWidth()-2)/3-m_iconCfg->image_ltrb[0]-m_iconCfg->image_ltrb[2];
    if (w<1)w=1;
    double wsc=(r->right-r->left)/(double)w;

    int h=m_iconCfg->olimage->getHeight()-2-m_iconCfg->image_ltrb[1]-m_iconCfg->image_ltrb[3];
    if (h<1)h=1;
    double hsc=(r->bottom-r->top)/(double)h;

    r->left-=(int) (m_iconCfg->image_ltrb[0]*wsc);
    r->top-=(int) (m_iconCfg->image_ltrb[1]*hsc);
    r->right+=(int) (m_iconCfg->image_ltrb[2]*wsc);
    r->bottom+=(int) (m_iconCfg->image_ltrb[3]*hsc);
  }
}
void WDL_VirtualIconButton_PreprocessSkinConfig(WDL_VirtualIconButton_SkinConfig *a)
{
  if (a && a->image && a->olimage)
  {
    a->image_ltrb_used=false;
    int lext=0,rext=0,bext=0,text=0;

    int w=a->olimage->getWidth();
    int h=a->olimage->getHeight();

    if (LICE_GetPixel(a->olimage,0,0)==LICE_RGBA(255,0,255,255)&&
        LICE_GetPixel(a->olimage,w-1,h-1)==LICE_RGBA(255,0,255,255))
    {
      int x;
      for (x = 1; x < w/3 && LICE_GetPixel(a->olimage,x,0)==LICE_RGBA(255,0,255,255); x ++);
      lext=x-1;
      for (x = 1; x < h && LICE_GetPixel(a->olimage,0,x)==LICE_RGBA(255,0,255,255); x ++);
      text=x-1;

      for (x = w-2; x > (w*2/3) && LICE_GetPixel(a->olimage,x,h-1)==LICE_RGBA(255,0,255,255); x --);
      rext=w-2-x;
      for (x = h-2; x > text && LICE_GetPixel(a->olimage,w-1,x)==LICE_RGBA(255,0,255,255); x --);
      bext=h-2-x;
      if (lext||text||rext||bext)
      {
        a->image_ltrb_used=true;
        a->image_ltrb[0]=lext;
        a->image_ltrb[1]=text;
        a->image_ltrb[2]=rext;
        a->image_ltrb[3]=bext;
      }
    }

  }
}