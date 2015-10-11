// MidiDevice.cpp: implementation of the CMidiDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MidiDevice.h"
int dklfjd=0;
Ctest4Dlg *mDlg;
void CALLBACK MidiInCallback(HMIDIIN hMidiIn,WORD wMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2)
{
	dklfjd++;
	
	HGLOBAL hGMem;
	LPVOID lpMidi;
	MSGPARAMS* pMsgParams;
	CMidiDevice* pMidiDev = (CMidiDevice*)dwInstance;
	
	switch(wMsg) 
	{
		case MM_MIM_DATA:
		{
			//printf("通道信息! %d\n",dklfjd);
			hGMem = GlobalAlloc(GMEM_SHARE, sizeof(MSGPARAMS));
			lpMidi = GlobalLock(hGMem);
			pMsgParams = (MSGPARAMS *)lpMidi;
			pMsgParams->message = (BYTE)(dwParam1);
			pMsgParams->data1 = (BYTE)(dwParam1 >> 8);
			pMsgParams->data2 = (BYTE)(dwParam1 >> 16);
			pMsgParams->time = dwParam2;
			if(pMsgParams->message!=-2){

				printf("%x %x %x %d  %d\n", pMsgParams->message, pMsgParams->data1, pMsgParams->data2, pMsgParams->time, dwParam1);
					mDlg->Draw(dwParam1,pMsgParams->time);
				MIDIRECORDEVENT *pEvent = NULL;
				pEvent = new MIDIRECORDEVENT;
				if(pEvent != NULL)
				{				
					pEvent->u.dwEvent = dwParam1;
					pEvent->dwTime = dwParam2;
					pMidiDev->AddMidiRecordEvent(pEvent);

					if(pMidiDev->m_pWnd != NULL && pMidiDev->m_bNotifyWindow)
					{
						pMidiDev->m_pWnd->PostMessage(WM_USER_MIDI_IN, (WPARAM)dwParam1, 0L);
					}
				}
				GlobalUnlock(hGMem);
			
		}
		}
		break;

		case MM_MIM_LONGDATA:
			printf("系统信息--------------------------》! %d\n",dklfjd);
			break;
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMidiDevice::CMidiDevice()
{
	m_pWnd = NULL;
	m_hMidiIn = NULL;
	m_hMidiOut = NULL;
	m_bNotifyWindow = FALSE;
	m_StateIn = CLOSEDIN;
	m_StateOut = CLOSEDOUT;
	m_nTotalNumOfDevIn = 0;
	m_nTotalNumOfDevOut = 0;
}
CMidiDevice::CMidiDevice(Ctest4Dlg* pDlg)
{
	m_pWnd = NULL;
	m_hMidiIn = NULL;
	m_hMidiOut = NULL;
	m_bNotifyWindow = FALSE;
	mDlg = pDlg;
	m_StateIn = CLOSEDIN;
	m_StateOut = CLOSEDOUT;
	m_nTotalNumOfDevIn = 0;
	m_nTotalNumOfDevOut = 0;
}

CMidiDevice::~CMidiDevice()
{
}

void CMidiDevice::EnumMidiDev()
{
	int i = 0; 
	MIDIINCAPS midiInDevCaps;
	MIDIOUTCAPS midiOutDevCaps;

	// get the number of midi device in and out
	m_nTotalNumOfDevIn = midiInGetNumDevs();
	m_nTotalNumOfDevOut = midiOutGetNumDevs();

	// init the device name and device id
	for(i = 0; i < MAX_MIDI_DEV; i++)
	{
		m_midiDevIn[i].nDevID = -1;
		m_midiDevOut[i].nDevID = -1;
		memset(m_midiDevIn[i].szPnameIn, 0, MAXPNAMELEN);
		memset(m_midiDevOut[i].szPnameOut, 0, MAXPNAMELEN);
	}

	// get the device name and device id of midi device in
	for(i = 0; i < m_nTotalNumOfDevIn; i++)
	{
		midiInGetDevCaps(i, &midiInDevCaps, sizeof(MIDIINCAPS));
		memcpy(m_midiDevIn[i].szPnameIn, midiInDevCaps.szPname, MAXPNAMELEN);
		m_midiDevIn[i].nDevID = i;
	}

	// get the device name and device id of midi device out
	for(i = 0; i < m_nTotalNumOfDevOut; i++)
	{
		midiOutGetDevCaps(i, &midiOutDevCaps, sizeof(MIDIOUTCAPS));
		memcpy(m_midiDevOut[i].szPnameOut, midiOutDevCaps.szPname, MAXPNAMELEN);
		m_midiDevOut[i].nDevID = i;
	}
}

UINT CMidiDevice::GetNumOfMidiDevIn()
{
	return m_nTotalNumOfDevIn;
}

UINT CMidiDevice::GetNumOfMidiDevOut()
{
	return m_nTotalNumOfDevOut;
}

INT CMidiDevice::GetDevInID()
{
	UINT uDeviceInID;
    MMRESULT Result = midiInGetID(m_hMidiIn, &uDeviceInID);
    if(Result != MMSYSERR_NOERROR)
    {
		return -1;
	}

	return uDeviceInID;
}

INT CMidiDevice::GetDevOutID()
{
	UINT uDeviceOutID;
    MMRESULT Result = midiOutGetID(m_hMidiOut, &uDeviceOutID);
    if(Result != MMSYSERR_NOERROR)
    {
		return -1;
	}

	return uDeviceOutID;
}

void CMidiDevice::GetDevInNames(CStringArray &strArrDevIn)
{
	strArrDevIn.RemoveAll();
	for(int i  = 0; i < m_nTotalNumOfDevIn; i++)
	{
		strArrDevIn.Add((LPCTSTR)(m_midiDevIn[i].szPnameIn));
	}
}

void CMidiDevice::GetDevOutNames(CStringArray &strArrDevOut)
{
	strArrDevOut.RemoveAll();
	for(int i = 0; i < m_nTotalNumOfDevOut; i++)
	{
		strArrDevOut.Add((LPCTSTR)(m_midiDevOut[i].szPnameOut));
	}
}

BOOL CMidiDevice::IsMidiInOpen()
{
	return (m_StateIn == OPENEDIN);
}

BOOL CMidiDevice::IsMidiOutOpen()
{
	return (m_StateOut == OPENEDOUT);
}

BOOL CMidiDevice::OpenDevIn(UINT uDeviceInID)
{
	// makes sure the previous device is closed
    CloseDevIn();

    // open midi input device
    MMRESULT Result = midiInOpen(&m_hMidiIn, 
		                  uDeviceInID, 
						  (DWORD)MidiInCallback,
					      (DWORD)this,
					      CALLBACK_FUNCTION
					      );

    // change current state
    if(Result == MMSYSERR_NOERROR)
    {
        m_StateIn = OPENEDIN;
    }
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CMidiDevice::OpenDevOut(UINT uDeviceOutID)
{
	// makes sure the previous device is closed
    CloseDevOut();

    // open midi output device
    MMRESULT Result = midiOutOpen(&m_hMidiOut, 
		                  uDeviceOutID, 
                          0L,
                          0L,
                          0L
						  );

    // change current state
    if(Result == MMSYSERR_NOERROR)
    {
        m_StateOut = OPENEDOUT;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CMidiDevice::CloseDevIn()
{
	// if the device is recording then stop recording
    if(m_StateIn == RECORDING)
    {
        StopRecording();
    }

    // if the device has opened...
    if(m_StateIn == OPENEDIN)
    {
        // close the device
        MMRESULT Result = midiInClose(m_hMidiIn);
        if(Result != MMSYSERR_NOERROR)
        {
            return FALSE;
        }

        // change the state
        m_StateIn = CLOSEDIN;
    }

	return TRUE;
}

BOOL CMidiDevice::CloseDevOut()
{
    if(m_StateOut == OPENEDOUT)
    {
        // change current state
        m_StateOut = CLOSEDOUT;

        // close the midi output device
        MMRESULT Result = midiOutClose(m_hMidiOut);
		if(Result != MMSYSERR_NOERROR)
        {
            return FALSE;
        }
    }

	return TRUE;
}

void CMidiDevice::StopRecording()
{
    if(m_StateIn == RECORDING)
    {
        // change the state
        m_StateIn = OPENEDIN;

        // reset the midi input device
        midiInReset(m_hMidiIn);

		// convert recorded time to relative minisecond time
		ConvertRecordTime();
    }
}

void CMidiDevice::StartRecording()
{
    if(m_StateIn == OPENEDIN)
    {
		printf("StartRecording:if(m_StateIn == OPENEDIN)\n");
        // change the state
        m_StateIn = RECORDING;

		// release the recorded buffer
		ClearAllRecordEvent();

        // start recording
        MMRESULT Result = midiInStart(m_hMidiIn);
        if(Result != MMSYSERR_NOERROR)
        {
			printf("error:if(Result != MMSYSERR_NOERROR)\n");
            m_StateIn = OPENEDIN;
        }
    }
}

BOOL CMidiDevice::IsRecording()
{
	return (m_StateIn == RECORDING);
}

void CMidiDevice::AddMidiRecordEvent(MIDIRECORDEVENT *pEvent)
{
	if(pEvent)
	{
		m_lstEventIn.AddTail(pEvent);
	}
}

BOOL CMidiDevice::SaveRecEventToFile(const LPCTSTR strFilePath)
{
	CFile file;
	int i = 0, nTrkLen = 0;
	BYTE b = 0;
	UCHAR *pData = NULL;

	union 
	{
		char bData[4];
		DWORD dwData;
	}u;

	// if there is no event in the midi file
	if(m_lstEventIn.IsEmpty())
	{
		return FALSE;
	}

	// if open the midi file failed
	if(!file.Open(strFilePath, CFile::modeCreate | CFile::modeReadWrite))
	{
		return FALSE;
	}
	
	BYTE chHead[] =
	{	
		0x4D,0x54,0x68,0x64,  // MThd
		0x00,0x00,0x00,0x06,  // head length
		0x00,0x01,            // file format
		0x00,0x01,            // track count
		0x00,0x78,            // time division
		0x4D,0x54,0x72,0x6B   // 音轨头
	};
	
	// write head chunk
	file.Write(chHead, sizeof(chHead));

	// init the track length to zero and modify it later
	for(i = 0; i <= 3; i++)
	{//音轨长度
		file.Write(&b, 1);
	}

	// write Meta tempo, set the tick to 1ms
	BYTE chTempo[] = {0x00, 0xFF, 0x51, 0x03, 0x07, 0xa1, 0x20};
	file.Write(chTempo, sizeof(chTempo));

	// think that the max number of bytes in every midi event 
	// is 7: 4 delta time bytes and 3 midi message bytes
	u.dwData = 7 * m_lstEventIn.GetCount();
	pData = new UCHAR[u.dwData];
	if(!pData)
	{
		return FALSE;
	}

	UCHAR* pCur = pData;
	POSITION pos = m_lstEventIn.GetHeadPosition();
	MIDIRECORDEVENT *pMsg = NULL;
	while(pos)
	{
		pMsg = (MIDIRECORDEVENT *)m_lstEventIn.GetNext(pos);
		
		// get delta time(ticks)
		for(i = 3; i >= 0; i--)
		{
			UCHAR uc = (UCHAR)(pMsg->dwTime >> (i * 8));
			if(uc != 0 || i == 0)
			{
				*pCur = uc;
				pCur++;
				nTrkLen++;
			}
		}
		
		int nByt = 3;
		BYTE bEventType = pMsg->u.bData[i];

		// Cn xx  and Dn xx only two bytes
		if(bEventType == 0xc0 || bEventType == 0xd0)
		{
			nByt = 2;	
		}

		// 9n, Bn, En are three bytes 
		for(i = 0; i < nByt; i++)
		{
			*pCur = pMsg->u.bData[i];
			pCur++;
			nTrkLen++;
		}
	}

	u.dwData = nTrkLen;
	
	// write all midi events
	file.Write(pData, u.dwData);
	
	// write the end mark of track
	UCHAR chEnd[] = {0x00, 0xFF, 0x2F, 0x00};
	file.Write(chEnd, sizeof(chEnd));

	// change the track length
	file.Seek(18, file.begin);
	
	u.dwData = u.dwData + 7 + 4;
	for(i = 3; i >= 0; i--)
	{
		file.Write(&u.bData[i], 1);
	}

	file.Close();

	return TRUE;
}

void CMidiDevice::ClearAllRecordEvent()
{
	MIDIRECORDEVENT *pTemp = NULL;
	POSITION pos = m_lstEventIn.GetHeadPosition();
	
	while(pos)
	{
		pTemp = (MIDIRECORDEVENT *)m_lstEventIn.GetNext(pos);
		delete pTemp;
	}

	m_lstEventIn.RemoveAll();
}

DWORD CMidiDevice::ConvertDelta(DWORD dwValue)
{
	int i, nCount = 0;
	DWORD dwReturn = 0;
	BYTE ubTemp[4] = {0x00, 0x80, 0x80, 0x80};

	for(i = 0; i < 4; i++)
	{
		nCount++;
		ubTemp[i] |= BYTE(dwValue & 0x7F);
		dwValue >>= 7;					
		if(dwValue == 0) break;
	}

	for(i = nCount - 1; i >= 0; i--)
	{
		dwReturn |= ubTemp[i];
		dwReturn <<= 8;
	}
	dwReturn >>= 8;

	return dwReturn;
}

void CMidiDevice::ConvertRecordTime()
{
	if(!m_lstEventIn.IsEmpty())
	{
		int nMod = 0;
		POSITION pos = m_lstEventIn.GetHeadPosition();
		MIDIRECORDEVENT * pEvent = NULL;
		pEvent = (MIDIRECORDEVENT *)m_lstEventIn.GetHead();
		DWORD dwStartTime = pEvent->dwTime;
		DWORD dwNewTime = dwStartTime;
		
		while(pos)
		{
			pEvent = (MIDIRECORDEVENT *)m_lstEventIn.GetNext(pos);
			dwNewTime = pEvent->dwTime;

			// get the delta time(ms)
			pEvent->dwTime -= dwStartTime;

			// convert the delta time to ticks format
			// BMP = 6000000 / MicroTempo = 120  ->
			// Microtempo = 500000 us per quarter note ->
			// division = 120 ticks per quarter note ->
			// 1ms = 12/50 ticks
			pEvent->dwTime = (pEvent->dwTime * 12 / 50);

			// accumulate the errors
			nMod += ((pEvent->dwTime * 12) % 50);
			if(nMod >= 50)
			{
				nMod -= 50;
				pEvent->dwTime += 1;	
			}

			// convert the delta time to special format: alterable data length
			pEvent->dwTime = ConvertDelta(pEvent->dwTime);

			dwStartTime = dwNewTime;
		}
	}
}

BOOL CMidiDevice::GetDevInCaps(UINT uDeviceInId, MIDIINCAPS &Caps)
{
	MMRESULT Result = midiInGetDevCaps(uDeviceInId, &Caps, sizeof Caps);
    if(Result != MMSYSERR_NOERROR)
    {
        return FALSE;
    }

	return TRUE;
}

BOOL CMidiDevice::GetDevOutCaps(UINT uDeviceOutId, MIDIOUTCAPS &Caps)
{
	MMRESULT Result = midiOutGetDevCaps(uDeviceOutId, &Caps, sizeof Caps);
    if(Result != MMSYSERR_NOERROR)
    {
        return FALSE;
    }

	return TRUE;
}

BOOL CMidiDevice::SendShortMsg(DWORD dwMsg)
{
	if(m_StateOut == OPENEDOUT)
    {
        MMRESULT Result = ::midiOutShortMsg(m_hMidiOut, dwMsg);
        if(Result != MMSYSERR_NOERROR)
        {
            return FALSE;
        }

		return TRUE;
    }

	return FALSE;
}

BOOL CMidiDevice::SendLongMsg(LPBYTE pSys, DWORD dwMsgLen)
{
	UINT err;
	MIDIHDR hdr;
	
	if(m_StateOut == OPENEDOUT)
	{
		// lock buffer and store pointer in MIDIHDR
		hdr.lpData = (LPSTR)pSys;
		
		if(hdr.lpData)
		{
			// store its size in the MIDIHDR
			hdr.dwBufferLength = dwMsgLen;
			
			// flags must be set to 0
			hdr.dwFlags = 0;
			
			// prepare the buffer and MIDIHDR
			err = midiOutPrepareHeader(m_hMidiOut, &hdr, sizeof(MIDIHDR));
			
			if(!err)
			{
				// send the buffer to midi out device
				err = midiOutLongMsg(m_hMidiOut, &hdr, sizeof(MIDIHDR));
				if(err)
				{
					return FALSE;
				}

				// unprepare the buffer and MIDIHDR
				while(MIDIERR_STILLPLAYING == midiOutUnprepareHeader(m_hMidiOut, &hdr, sizeof(MIDIHDR)))
				{
					// should put a delay in here rather than a busy-wait
					Sleep(5);
				}
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CMidiDevice::AllNotesOff(UINT uCh)
{
	DWORD dwEvent = 0;

	if(uCh >= 0 && uCh < 16)
	{
		// Ctrl 123
		dwEvent = 0xB0 | uCh | ((DWORD)0x7B << 8) | ((DWORD)(0x0) << 16);
		midiOutShortMsg(m_hMidiOut, dwEvent);
	}
}

void CMidiDevice::AllSoundOff(UINT uCh)
{
	DWORD dwEvent = 0;

	if(uCh >= 0 && uCh < 16)
	{	
		// Ctrl 120	
		dwEvent = 0xB0 | uCh | ((DWORD)0x78 << 8);
		midiOutShortMsg(m_hMidiOut, dwEvent);
	}
}

void CMidiDevice::ProgramChange(UINT uCh, BYTE bVal)
{
	DWORD dwEvent = 0;

	if(uCh >= 0 && uCh < 16)
	{
		dwEvent = 0xC0 | uCh | ((DWORD)bVal << 8);
		midiOutShortMsg(m_hMidiOut, dwEvent);
	}
}

void CMidiDevice::BankSelect(UINT uCh, BYTE bVal)
{
	DWORD dwEvent = 0;

	// check the parameters
	if(uCh >= 0 && uCh < 16)
	{
		// Ctrl 0
		dwEvent = 0xB0 | uCh | ((DWORD)0x00 << 8) | ((DWORD)bVal << 16);
		midiOutShortMsg(m_hMidiOut, dwEvent);
	}
}
	
void CMidiDevice::SetVolume(UINT uCh, BYTE bVal)
{
	DWORD dwEvent = 0;

	if(uCh >= 0 && uCh < 16)
	{
		// Ctrl 07
		dwEvent = 0xB0 | uCh | ((DWORD)0x07 << 8) | ((DWORD)bVal << 16);
		midiOutShortMsg(m_hMidiOut, dwEvent);
	}
}

void CMidiDevice::SetPan(UINT uCh, BYTE bVal)
{
	DWORD dwEvent = 0;

	if(uCh >= 0 && uCh < 16)
	{
		// Ctrl 10
		dwEvent = 0xB0 | uCh | ((DWORD)0x0a << 8) | ((DWORD)bVal << 16);
		midiOutShortMsg(m_hMidiOut, dwEvent);
	}
}
	
void CMidiDevice::SetReverbType(UINT uCh, BYTE bVal)
{
	DWORD dwEvent = 0;	

	if(uCh >= 0 && uCh < 16)
	{
		if(bVal >= 0 && bVal <= 7)
		{
			// Ctrl 80
			dwEvent = 0xB0 | uCh | ((DWORD)0x50 << 8) | ((DWORD)bVal << 16);
			midiOutShortMsg(m_hMidiOut, dwEvent);
		}
	}
}

void CMidiDevice::SetChorusType(UINT uCh, BYTE bVal)
{
	DWORD dwEvent = 0;	

	if(uCh >= 0 && uCh < 16)
	{
		if(bVal >= 0 && bVal <= 7)
		{
			// Ctrl 81
			dwEvent = 0xB0 | uCh | ((DWORD)0x51 << 8) | ((DWORD)bVal << 16);
			midiOutShortMsg(m_hMidiOut, dwEvent);
		}
	}
}

void CMidiDevice::SetReverbLevel(UINT uCh, BYTE bVal)
{
	DWORD dwEvent = 0;

	if(uCh >= 0 && uCh < 16)
	{
		// Ctrl 91
		dwEvent = 0xB0 | uCh | ((DWORD)0x5b << 8) | ((DWORD)bVal << 16);
		midiOutShortMsg(m_hMidiOut, dwEvent);
	}
}

void CMidiDevice::SetChorusLevel(UINT uCh, BYTE bVal)
{
	DWORD dwEvent = 0;

	if(uCh >= 0 && uCh < 16)
	{
		// Ctrl 93
		dwEvent = 0xB0 | uCh | ((DWORD)0x5d << 8) | ((DWORD)bVal << 16);
		midiOutShortMsg(m_hMidiOut, dwEvent);
	}
}
