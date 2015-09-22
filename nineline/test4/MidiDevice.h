// MidiDevice.h: interface for the CMidiDevice class.
//
//////////////////////////////////////////////////////////////////////

#define MAX_MIDI_DEV (64)
#define WM_USER_MIDI_IN (2009+02)

#include "windows.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

struct MSGPARAMS
{
	char message;
	char data1;
	char data2;
	long time;
};

struct MIDIRECORDEVENT
{
	DWORD dwTime;
	
	union
	{
		DWORD dwEvent;
		UCHAR bData[4];
	}u;
};

class CMidiDevice  
{
public:
	CMidiDevice();
	virtual ~CMidiDevice();

	typedef struct
	{
		int nDevID;  // the midi device number
		char szPnameIn[MAXPNAMELEN];  // the name of midi device in				
	}MIDI_DEVICE_IN;

	typedef struct
	{
		int nDevID;  // the midi device number
		char szPnameOut[MAXPNAMELEN];  // the name of midi device out			
	}MIDI_DEVICE_OUT;

	CWnd *m_pWnd;
	BOOL m_bNotifyWindow;  // notify the window or not when receiving midi
	HMIDIIN m_hMidiIn;  // the handle of midi device in
	HMIDIOUT m_hMidiOut;  // the handle of midi device out
	int m_nTotalNumOfDevIn;  // the total number of midi device in
	int m_nTotalNumOfDevOut;  // the total number of midi device out
	MIDI_DEVICE_IN m_midiDevIn[MAX_MIDI_DEV];  // use to save the information of midi device in
	MIDI_DEVICE_OUT m_midiDevOut[MAX_MIDI_DEV];  // use to save the information of midi device out
	CPtrList m_lstEventIn;  //a list for link the events of recording
 
	void EnumMidiDev();  // enumerate all midi device
	INT GetDevInID();  // get the ID for the midi in device
	UINT GetNumOfMidiDevIn();  // get the total number of midi device in
	BOOL IsMidiInOpen();  // see if midi device in opened or not
	BOOL OpenDevIn(UINT uDeviceInID);  // open the midi device in
	BOOL CloseDevIn();  // close midi device in
	void StopRecording();  // stop to record midi
	void StartRecording();  // start to record midi
	void AddMidiRecordEvent(MIDIRECORDEVENT *pEvent);  // add a record midi event to list
	void ConvertRecordTime();  // convert the recorded time to relative minisecond time
	DWORD ConvertDelta(DWORD dwValue);  // convert the delta time format
	BOOL SaveRecEventToFile(const LPCTSTR strFilePath);  // save the recorded midi in event to a midi file
	void ClearAllRecordEvent();  // clear all recorded midi in event
	BOOL IsRecording();  // see if midi in device is recording or not
	BOOL GetDevInCaps(UINT uDeviceInId, MIDIINCAPS &Caps);  // get the capabilities of midi in device

	INT GetDevOutID();  // get the ID for the midi out device
	UINT GetNumOfMidiDevOut();  // get the total number of midi device out
	BOOL IsMidiOutOpen();  // see if midi device out opened or not
	BOOL OpenDevOut(UINT uDeviceOutID);  // open the midi device out
	BOOL CloseDevOut();  // close midi device out
	BOOL GetDevOutCaps(UINT uDeviceOutId, MIDIOUTCAPS &Caps);  // get the capabilities of midi out device
	HMIDIOUT GetMidiOutHandle(){ return m_hMidiOut; }  // get the handle of midi device in
	HMIDIIN GetMidiInHandle(){ return m_hMidiIn; }  // get the handle of midi device in
	void GetDevInNames(CStringArray &strArrDevIn);	// get the names of all device in 
	void GetDevOutNames(CStringArray &strArrDevOut);	// get the names of all device out
	
	BOOL SendShortMsg(DWORD dwMsg);  // send short message
	BOOL SendLongMsg(LPBYTE pSys, DWORD dwMsgLen);  // send long message
	void AllNotesOff(UINT uCh);  // turn all notes off on channel n
	void AllSoundOff(UINT uCh);  // abrupt stop of sound on channel n
	void ProgramChange(UINT uCh, BYTE bVal);  // change the program(patch) on channel n
	void BankSelect(UINT uCh, BYTE bVal);  // bank select on channel n
	void SetVolume(UINT uCh, BYTE bVal);  // set volume on channel n
	void SetPan(UINT uCh, BYTE bVal);  // set pan on channel n
	void SetReverbType(UINT uCh, BYTE bVal);  // reverb program on channel n
	void SetChorusType(UINT uCh, BYTE bVal);  // chorus program on channel n
	void SetReverbLevel(UINT uCh, BYTE bVal);  // reverb send level on channel n
	void SetChorusLevel(UINT uCh, BYTE bVal);  // chorus send level on channel n

private:
	enum StateIn{ CLOSEDIN , OPENEDIN, RECORDING } m_StateIn;
	enum StateOut{ CLOSEDOUT, OPENEDOUT } m_StateOut;
	
};
