/*
 *  Copyright (C) 2005-2021 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "GUIColorButtonControl.h"

#include "GUIInfoManager.h"
#include "LocalizeStrings.h"
#include "input/Key.h"
#include "utils/ColorUtils.h"
#include "utils/StringUtils.h"

CGUIColorButtonControl::CGUIColorButtonControl(int parentID,
                                               int controlID,
                                               float posX,
                                               float posY,
                                               float width,
                                               float height,
                                               const CTextureInfo& textureFocus,
                                               const CTextureInfo& textureNoFocus,
                                               const CLabelInfo& labelInfo,
                                               const CTextureInfo& colorMask,
                                               const CTextureInfo& colorDisabledMask)
  : CGUIButtonControl(
        parentID, controlID, posX, posY, width, height, textureFocus, textureNoFocus, labelInfo),
    m_imgColorMask(CGUITexture::CreateTexture(posX, posY, 16, 16, colorMask)),
    m_imgColorDisabledMask(CGUITexture::CreateTexture(posX, posY, 16, 16, colorDisabledMask)),
    m_labelInfo(posX, posY, width, height, labelInfo, CGUILabel::OVER_FLOW_SCROLL)
{
  m_colorPosX = 0;
  m_colorPosY = 0;
  m_imgColorMask->SetAspectRatio(CAspectRatio::AR_KEEP);
  m_imgColorDisabledMask->SetAspectRatio(CAspectRatio::AR_KEEP);
  m_imgBoxColor = UTILS::COLOR::NONE;
  ControlType = GUICONTROL_COLORBUTTON;
}

CGUIColorButtonControl::CGUIColorButtonControl(const CGUIColorButtonControl& control)
  : CGUIButtonControl(control),
    m_imgColorMask(control.m_imgColorMask->Clone()),
    m_imgColorDisabledMask(control.m_imgColorDisabledMask->Clone()),
    m_colorPosX(control.m_colorPosX),
    m_colorPosY(control.m_colorPosY),
    m_labelInfo(control.m_labelInfo)
{
}

void CGUIColorButtonControl::Render()
{
  CGUIButtonControl::Render();
  RenderInfoText();
  if (IsDisabled())
    m_imgColorDisabledMask->Render();
  else
    m_imgColorMask->Render();
}

void CGUIColorButtonControl::Process(unsigned int currentTime, CDirtyRegionList& dirtyregions)
{
  ProcessInfoText(currentTime);
  m_imgColorMask->Process(currentTime);
  m_imgColorDisabledMask->Process(currentTime);
  CGUIButtonControl::Process(currentTime, dirtyregions);
}

bool CGUIColorButtonControl::OnAction(const CAction& action)
{
  return CGUIButtonControl::OnAction(action);
}

bool CGUIColorButtonControl::OnMessage(CGUIMessage& message)
{
  return CGUIButtonControl::OnMessage(message);
}

void CGUIColorButtonControl::AllocResources()
{
  CGUIButtonControl::AllocResources();
  m_imgColorMask->AllocResources();
  m_imgColorDisabledMask->AllocResources();
  SetPosition(m_posX, m_posY);
}

void CGUIColorButtonControl::FreeResources(bool immediately)
{
  CGUIButtonControl::FreeResources(immediately);
  m_imgColorMask->FreeResources(immediately);
  m_imgColorDisabledMask->FreeResources(immediately);
}

void CGUIColorButtonControl::DynamicResourceAlloc(bool bOnOff)
{
  CGUIControl::DynamicResourceAlloc(bOnOff);
  m_imgColorMask->DynamicResourceAlloc(bOnOff);
  m_imgColorDisabledMask->DynamicResourceAlloc(bOnOff);
}

void CGUIColorButtonControl::SetInvalid()
{
  CGUIButtonControl::SetInvalid();
  m_imgColorMask->SetInvalid();
  m_imgColorDisabledMask->SetInvalid();
  m_labelInfo.SetInvalid();
}

void CGUIColorButtonControl::SetPosition(float posX, float posY)
{
  CGUIButtonControl::SetPosition(posX, posY);
  float colorPosX =
      m_colorPosX ? m_posX + m_colorPosX : (m_posX + m_width) - m_imgColorMask->GetWidth();
  float colorPosY =
      m_colorPosY ? m_posY + m_colorPosY : m_posY + (m_height - m_imgColorMask->GetHeight()) / 2;
  m_imgColorMask->SetPosition(colorPosX, colorPosY);
  m_imgColorDisabledMask->SetPosition(colorPosX, colorPosY);
}

void CGUIColorButtonControl::SetColorDimensions(float posX, float posY, float width, float height)
{
  m_colorPosX = posX;
  m_colorPosY = posY;
  if (width)
  {
    m_imgColorMask->SetWidth(width);
    m_imgColorDisabledMask->SetWidth(width);
  }
  if (height)
  {
    m_imgColorMask->SetHeight(height);
    m_imgColorDisabledMask->SetHeight(height);
  }
  SetPosition(GetXPosition(), GetYPosition());
}

void CGUIColorButtonControl::SetWidth(float width)
{
  CGUIButtonControl::SetWidth(width);
  SetPosition(GetXPosition(), GetYPosition());
}

void CGUIColorButtonControl::SetHeight(float height)
{
  CGUIButtonControl::SetHeight(height);
  SetPosition(GetXPosition(), GetYPosition());
}

std::string CGUIColorButtonControl::GetDescription() const
{
  return CGUIButtonControl::GetDescription();
}

void CGUIColorButtonControl::SetImageBoxColor(KODI::GUILIB::GUIINFO::CGUIInfoColor color)
{
  m_imgBoxColor = color;
}

void CGUIColorButtonControl::SetImageBoxColor(std::string hexColor)
{
  if (hexColor.empty())
    m_imgBoxColor = UTILS::COLOR::NONE;
  else
    m_imgBoxColor = UTILS::ConvertHexToColor(hexColor);
}

bool CGUIColorButtonControl::UpdateColors(const CGUIListItem* item)
{
  bool changed = CGUIButtonControl::UpdateColors(nullptr);
  changed |= m_imgColorMask->SetDiffuseColor(m_imgBoxColor, item);
  changed |= m_imgColorDisabledMask->SetDiffuseColor(m_diffuseColor);
  changed |= m_labelInfo.UpdateColors();
  return changed;
}

void CGUIColorButtonControl::RenderInfoText()
{
  m_labelInfo.Render();
}

void CGUIColorButtonControl::ProcessInfoText(unsigned int currentTime)
{
  CRect labelRenderRect = m_labelInfo.GetRenderRect();
  bool changed = m_labelInfo.SetText(StringUtils::Format("#{:8x}", m_imgBoxColor));
  // Set Label X position based on image mask control position
  changed = m_labelInfo.SetMaxRect(m_imgColorMask->GetXPosition() - 300, m_posY, 300, m_height);
  changed |= m_labelInfo.SetScrolling(HasFocus());
  changed |= m_labelInfo.SetAlign(XBFONT_RIGHT | (m_label.GetLabelInfo().align & XBFONT_CENTER_Y));

  // text changed - images need resizing
  if (m_minWidth && (m_labelInfo.GetRenderRect() != labelRenderRect))
    SetInvalid();

  changed |= m_labelInfo.SetColor(GetTextColor());
  changed |= m_labelInfo.Process(currentTime);
  if (changed)
    MarkDirtyRegion();
}

CGUILabel::COLOR CGUIColorButtonControl::GetTextColor() const
{
  if (IsDisabled())
    return CGUILabel::COLOR_DISABLED;
  if (HasFocus())
    return CGUILabel::COLOR_FOCUSED;
  return CGUILabel::COLOR_TEXT;
}
