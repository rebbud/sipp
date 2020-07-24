/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307, USA
 *
 *  Author : Deon van der Westhuysen - June 2012 - Vodacom PTY LTD
 */

#ifndef __RTPSTREAM__
#define __RTPSTREAM__

#define RTPSTREAM_MAX_FILENAMELEN 256
#include "jlsrtp.hpp"
#include <tr1/unordered_map>

#define RTPSTREAM_MAX_FILENAMELEN 256
#define RTPSTREAM_MAX_PAYLOADNAME 256

struct threaddata_t;
struct taskentry_t;
using namespace std;
typedef struct _SrtpAudioInfoParams
{
    bool audio_found;
    int primary_audio_cryptotag;
    char primary_audio_cryptosuite[24];
    char primary_audio_cryptokeyparams[42];
    int secondary_audio_cryptotag;
    char secondary_audio_cryptosuite[24];
    char secondary_audio_cryptokeyparams[42];
    bool primary_unencrypted_audio_srtp;
    bool secondary_unencrypted_audio_srtp;
} SrtpAudioInfoParams;

typedef struct _SrtpVideoInfoParams
{
    bool video_found;
    int primary_video_cryptotag;
    char primary_video_cryptosuite[24];
    char primary_video_cryptokeyparams[42];
    int secondary_video_cryptotag;
    char secondary_video_cryptosuite[24];
    char secondary_video_cryptokeyparams[42];
    bool primary_unencrypted_video_srtp;
    bool secondary_unencrypted_video_srtp;
} SrtpVideoInfoParams;

struct threaddata_t;

struct taskentry_t
{
    threaddata_t         *parent_thread;
    unsigned long        nextwake_ms;
    volatile int         flags;
    /* DUB ms */
    unsigned long long   ts;
    unsigned long long   ts2;	

    /* rtp stream information */
    unsigned long long   last_timestamp;
    unsigned long long   last_timestamp2;
    unsigned short       seq;
    unsigned short       seq2;
    char                 payload_type;
    unsigned int         ssrc_id;
    unsigned int         ssrc_id2;

    /* current playback information */
    int                  loop_count;
    int                  loop_count2;
    char                 *file_bytes_start;
    char                 *file_bytes_start2;
    char                 *current_file_bytes;
    char                 *current_file_bytes2;
    int                  file_num_bytes;
    int                  file_num_bytes2;
    int                  file_bytes_left;
    int                  file_bytes_left2;
    /* playback timing information */
    int                  ms_per_packet;
    int                  bytes_per_packet;
    int                  timeticks_per_packet;
    int                  timeticks_per_ms;

    /* new file playback information */
    char                 new_payload_type;
    int                  new_loop_count;
    int                  new_file_size;
    int                  new_file_size2;
    char                 *new_file_bytes;
    char                 *new_file_bytes2;
    int                  new_ms_per_packet;
    int                  new_bytes_per_packet;
    int                  new_timeticks_per_packet;
    /* sockets for audio/video rtp_rtcp */
    int                  audio_rtp_socket;
    int                  audio_rtp_socket2;
    int                  audio_rtcp_socket;
    int                  audio_rtcp_socket2;

    /* rtp peer address structures */
    struct sockaddr_storage    remote_audio_rtp_addr;
    struct sockaddr_storage    remote_audio_rtp_addr2;

    struct sockaddr_storage    remote_audio_rtcp_addr;
    struct sockaddr_storage    remote_audio_rtcp_addr2;

    /* we will have a mutex per call. should we consider refactoring to */
    /* share mutexes across calls? makes the per-call code more complex */

    /* thread mananagment structures */
    pthread_mutex_t      mutex;

    /*DUB */
    SrtpAudioInfoParams  local_srtp_audio_params;
    SrtpAudioInfoParams  remote_srtp_audio_params;
    SrtpVideoInfoParams  local_srtp_video_params;
    SrtpVideoInfoParams  remote_srtp_video_params;


    SrtpAudioInfoParams  local_srtp_audio_params2;
    SrtpAudioInfoParams  remote_srtp_audio_params2;
    SrtpVideoInfoParams  local_srtp_video_params2;
    SrtpVideoInfoParams  remote_srtp_video_params2;

    int 		 txindex = -1;
};

struct rtpstream_callinfo_t
{
    taskentry_t* taskinfo;
    int audioport;
    int audioport2; 
    int videoport;
    char payload_name[RTPSTREAM_MAX_PAYLOADNAME]; 
};

struct rtpstream_actinfo_t
{
    char filename[RTPSTREAM_MAX_FILENAMELEN];
    char filename2[RTPSTREAM_MAX_FILENAMELEN];
    int loop_count;
    int bytes_per_packet;
    int ms_per_packet;
    int ticks_per_packet; /* need rework for 11.025 sample rate */
    int payload_type;
};

int rtpstream_new_call(rtpstream_callinfo_t *callinfo);
void rtpstream_end_call(rtpstream_callinfo_t *callinfo);
void rtpstream_shutdown(void);

int rtpstream_get_audioport(rtpstream_callinfo_t *callinfo);
int rtpstream_get_videoport(rtpstream_callinfo_t *callinfo);
void rtpstream_set_remote(rtpstream_callinfo_t* callinfo, int ip_ver, const char* ip_addr,
                          int audio_port,int audio_port2, int video_port);

int rtpstream_cache_file(char *filename);
void rtpstream_pause(rtpstream_callinfo_t *callinfo);
void rtpstream_resume(rtpstream_callinfo_t *callinfo);

int rtpstream_set_srtp_audio_local(rtpstream_callinfo_t *callinfo, vector<SrtpAudioInfoParams> &p);
int rtpstream_set_srtp_audio_remote(rtpstream_callinfo_t *callinfo, vector<SrtpAudioInfoParams> &p);
void rtpstream_playsrtp_ms(rtpstream_callinfo_t* callinfo, rtpstream_actinfo_t* actioninfo,vector<JLSRTP> & txUACAudio, vector<JLSRTP>& rxUACAudio);
#endif
