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
 */

#include "stdafx.h"
#include "PacketQueue.h"

// Queue a new packet at the end of the list
void CPacketQueue::Queue(Packet *pPacket)
{
  CAutoLock cAutoLock(this);

  if (pPacket) {
    if (pPacket->bAppendable && !pPacket->bDiscontinuity && !pPacket->pmt
      && pPacket->rtStart == Packet::INVALID_TIME && !IsEmpty()
      && m_queue.back()->rtStart != Packet::INVALID_TIME) {
        Packet* tail = m_queue.back();
        tail->Append(pPacket);

        return;
    }
  }
  m_queue.push_back(pPacket);
}

// Get a packet from the beginning of the list
Packet *CPacketQueue::Get()
{
  CAutoLock cAutoLock(this);

  if (m_queue.size() == 0) {
    return NULL;
  }
  Packet *pPacket = m_queue.front();
  m_queue.pop_front();
  return pPacket;
}

// Get the size of the queue
size_t CPacketQueue::Size()
{
  CAutoLock cAutoLock(this);

  return m_queue.size();
}

// Clear the List (all elements are free'ed)
void CPacketQueue::Clear()
{
  CAutoLock cAutoLock(this);

  std::deque<Packet *>::iterator it;
  for (it = m_queue.begin(); it != m_queue.end(); it++) {
    free(*it);
  }
  m_queue.clear();
}
