/*
 *      Copyright (C) 2010 Hendrik Leppkes
 *      http://www.1f0.de
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 *  Initial design and concept by Gabest and the MPC-HC Team, copyright under GPLv2
 *  Contributions by Ti-BEN from the XBMC DSPlayer Project, also under GPLv2
 */

#pragma once

#include <vector>
#include <string>
#include "PacketQueue.h"

#include "moreuuids.h"

#include "LAVFSplitter.h"

class CLAVFOutputPin
  : public CBaseOutputPin
  , protected CAMThread
{
public:
  CLAVFOutputPin(std::vector<CMediaType>& mts, LPCWSTR pName, CBaseFilter *pFilter, CCritSec *pLock, HRESULT *phr, CLAVFSplitter::StreamType pinType = CLAVFSplitter::unknown,const char* container = "", int nBuffers = 0);
  CLAVFOutputPin(LPCWSTR pName, CBaseFilter *pFilter, CCritSec *pLock, HRESULT *phr, CLAVFSplitter::StreamType pinType = CLAVFSplitter::unknown, const char* container = "", int nBuffers = 0);
  ~CLAVFOutputPin();

  DECLARE_IUNKNOWN;
  STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

  // IQualityControl
  STDMETHODIMP Notify(IBaseFilter* pSender, Quality q) { return E_NOTIMPL; }

  // CBaseOutputPin
  HRESULT CheckConnect(IPin* pPin);
  HRESULT DecideBufferSize(IMemAllocator* pAlloc, ALLOCATOR_PROPERTIES* pProperties);
  HRESULT CheckMediaType(const CMediaType* pmt);
  HRESULT GetMediaType(int iPosition, CMediaType* pmt);
  HRESULT Active();
  HRESULT Inactive();

  HRESULT DeliverBeginFlush();
  HRESULT DeliverEndFlush();

  HRESULT DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);

  size_t QueueCount();
  HRESULT QueuePacket(Packet *pPacket);
  HRESULT QueueEndOfStream();
  bool IsDiscontinuous();

  DWORD GetStreamId() { return m_streamId; };
  void SetStreamId(DWORD newStreamId) { m_streamId = newStreamId; };

  void SetNewMediaType(CMediaType pmt) { CAutoLock lock(&m_csMT); m_mts.clear(); m_mts.push_back(pmt); }
  void QueueMediaType(CMediaType pmt) { CAutoLock lock(&m_csMT); m_newMT = new CMediaType(pmt); }

  BOOL IsVideoPin() { return m_pinType == CLAVFSplitter::video; }
  BOOL IsAudioPin() { return m_pinType == CLAVFSplitter::audio; }
  BOOL IsSubtitlePin(){ return m_pinType == CLAVFSplitter::subpic; }

private:
  enum {CMD_EXIT};
  DWORD ThreadProc();

  HRESULT DeliverPacket(Packet *pPacket);

private:
  CCritSec m_csMT;
  std::vector<CMediaType> m_mts;
  CPacketQueue m_queue;

  std::string m_containerFormat;

  REFERENCE_TIME m_rtStart;

  // Flush control
  bool m_fFlushing, m_fFlushed;
  CAMEvent m_eEndFlush;

  HRESULT m_hrDeliver;

  int m_nBuffers;

  DWORD m_streamId;
  CMediaType *m_newMT;

  CLAVFSplitter::StreamType m_pinType;
};
