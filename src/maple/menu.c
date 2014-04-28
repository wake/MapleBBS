/*-------------------------------------------------------*/
/* menu.c	( NTHU CS MapleBBS Ver 3.00 )		 */
/*-------------------------------------------------------*/
/* target : menu/help/movie routines		 	 */
/* create : 95/03/29				 	 */
/* update : 97/03/29				 	 */
/*-------------------------------------------------------*/


#include "bbs.h"


extern UCACHE *ushm;
extern FCACHE *fshm;


#ifndef ENHANCED_VISIT
extern time_t brd_visit[];
#endif


/* ----------------------------------------------------- */
/* ���} BBS ��						 */
/* ----------------------------------------------------- */


#define	FN_RUN_NOTE_PAD	"run/note.pad"
#define	FN_RUN_NOTE_TMP	"run/note.tmp"


typedef struct
{
  time_t tpad;
  char msg[400];
}      Pad;


int
pad_view()
{
  int fd, count;
  Pad *pad;

  if ((fd = open(FN_RUN_NOTE_PAD, O_RDONLY)) < 0)
    return XEASY;

  count = 0;
  mgets(-1);

  for (;;)
  {
    pad = mread(fd, sizeof(Pad));
    if (!pad)
    {
      vmsg(NULL);
      break;
    }
    else if (!(count % 5))	/* itoc.020122: �� pad �~�L */
    {
      clear();
      move(0, 23);
      prints("�i �� �� �W �� �d �� �O �j                �� %d ��\n\n", count / 5 + 1);
    }

    outs(pad->msg);
    count++;

    if (!(count % 5))
    {
      move(b_lines, 0);
      outs("�Ы� [SPACE] �~���[��A�Ϋ���L�䵲���G ");
      /* itoc.010127: �ץ��b�������k����ΤU�A������|�����G�h��檺���D */

      if (vkey() != ' ')
	break;
    }
  }

  close(fd);
  return 0;
}


static int
pad_draw()
{
  int i, cc, fdr, color;
  FILE *fpw;
  Pad pad;
  char *str, buf[3][71];

  /* itoc.����: ���Q�ΰ��m�סA�Ӫ� */
  static char pcolors[6] = {31, 32, 33, 34, 35, 36};

  /* itoc.010309: �d���O���Ѥ��P���C�� */
  color = vans("�߱��C�� 1) \033[41m  \033[m 2) \033[42m  \033[m 3) \033[43m  \033[m "
    "4) \033[44m  \033[m 5) \033[45m  \033[m 6) \033[46m  \033[m [Q] ");

  if (color < '1' || color > '6')
    return XEASY;
  else
    color -= '1';

  do
  {
    buf[0][0] = buf[1][0] = buf[2][0] = '\0';
    move(MENU_XPOS, 0);
    clrtobot();
    outs("\n�Яd�� (�ܦh�T��)�A��[Enter]����");
    for (i = 0; (i < 3) &&
      vget(16 + i, 0, "�G", buf[i], 71, DOECHO); i++);
    cc = vans("(S)�s���[�� (E)���s�ӹL (Q)��F�H[S] ");
    if (cc == 'q' || i == 0)
      return 0;
  } while (cc == 'e');

  time(&pad.tpad);

  /* itoc.020812.����: �睊�����ɭԭn�`�N struct Pad.msg[] �O�_���j */
  str = pad.msg;
  sprintf(str, "�~�t\033[1;46m %s �� %s \033[m�u", cuser.userid, cuser.username);

  for (cc = strlen(str); cc < 60; cc += 2)
    strcpy(str + cc, "�w");
  if (cc == 60)
    str[cc++] = ' ';

  sprintf(str + cc,
    "\033[1;44m %s \033[m��\n"
    "�x  \033[1;%dm%-70s\033[m  �x\n"
    "�x  \033[1;%dm%-70s\033[m  �x\n"
    "��  \033[1;%dm%-70s\033[m  ��\n",
    Btime(&pad.tpad),
    pcolors[color], buf[0],
    pcolors[color], buf[1],
    pcolors[color], buf[2]);

  f_cat(FN_RUN_NOTE_ALL, str);

  if (!(fpw = fopen(FN_RUN_NOTE_TMP, "w")))
    return 0;

  fwrite(&pad, sizeof(pad), 1, fpw);

  if ((fdr = open(FN_RUN_NOTE_PAD, O_RDONLY)) >= 0)
  {
    Pad *pp;

    i = 0;
    cc = pad.tpad - NOTE_DUE * 60 * 60;
    mgets(-1);
    while (pp = mread(fdr, sizeof(Pad)))
    {
      fwrite(pp, sizeof(Pad), 1, fpw);
      if ((++i > NOTE_MAX) || (pp->tpad < cc))
	break;
    }
    close(fdr);
  }

  fclose(fpw);

  rename(FN_RUN_NOTE_TMP, FN_RUN_NOTE_PAD);
  pad_view();
  return 0;
}


static int
goodbye()
{
  /* itoc.010803: �q�i�������� */
  clear();
  film_out(FILM_GOODBYE, 0);

  switch (vans("G)�H���ӳu M)���i���� N)�d���O Q)�����H[Q] "))
  {
  /* lkchu.990428: ���w�אּ������ */
  case 'g':
  case 'y':
    break;    

  case 'm':
    m_sysop();
    break;

  case 'n':
    /* if (cuser.userlevel) */
    if (HAS_PERM(PERM_POST)) /* Thor.990118: �n��post�~��d��, �������e */
      pad_draw();
    break;

  case 'q':
  default:
    /* return XEASY; */
    return 0;	/* itoc.010803: �q�F FILM_GOODBYE �n��ø */
  }

#ifdef LOG_BMW
  bmw_log();			/* lkchu.981201: ���y�O���B�z */
#endif

  if (!(cuser.ufo & UFO_MOTD))	/* itoc.000407: �����e���@��²�� */
  {  
    clear();
    prints("�˷R�� \033[32m%s(%s)\033[m�A�O�ѤF�A�ץ��{�i %s �j\n"
      "�H�U�O�z�b���������U��ơG\n",
      cuser.userid, cuser.username, str_site);
    acct_show(&cuser, 0);
    vmsg(NULL);
  }
  
  u_exit("EXIT ");
  exit(0);
}


/* ----------------------------------------------------- */
/* help & menu processring				 */
/* ----------------------------------------------------- */


void
vs_head(title, mid)
  char *title, *mid;
{
  char buf[40], ttl[60];
  int spc, len;

  if (mid)	/* xxxx_head() ���O�� vs_head(title, str_site); */
  {
    clear();
  }
  else		/* menu() ���~�� vs_head(title, NULL); ��椤�L�� clear() */
  {
    move(0, 0);
    clrtoeol();
    mid = str_site;
  }

  len = d_cols + 69 - strlen(title) - strlen(currboard);

  /* 040209.Lacool:�s�W�H�U 6 ��A���s���U��ɷ|�۰���� */
  if (HAS_PERM(PERM_ALLREG) && rec_num(FN_RUN_RFORM, 1))
  {
    mid = "\033[41;37;5m ���ϥΪ̵��U \033[0;1;33;44m";
    spc = 14;
  }

  else if (HAS_STATUS(STATUS_BIFF))
  {
    mid = "\033[5;41m �l�t�ӫ��a�F \033[m";
    spc = 14;
  }
  else
  {
    if ((spc = strlen(mid)) > len)
    {
      spc = len;
      memcpy(ttl, mid, spc);
      mid = ttl;
      mid[spc] = '\0';
    }
  }

  /* 041023.Lacool:�s�W���H�ӽЬݪO�ɦ۰ʰ{�{���(�ȧP�_[�̫�@��]�ӽ�) */
  if (HAS_PERM(PERM_ALLBOARD))
  {
    char *folder = "run/newbrd/"FN_DIR;
    int num = rec_num(folder, sizeof(NBRD));
    NBRD nbrd;

    /* �ݳ̫�@�g�O�_�w�g�}�O */
    if (num > 0 && !rec_get(folder, &nbrd, sizeof(NBRD), num - 1) &&
      !(nbrd.mode & NBRD_FINISH))
    {
      mid = "\033[5;41m ���H�ӽЬݪO \033[m";
      spc = 14;
    }
  }

  spc = 2 + len - spc;
  len = 1 - spc & 1;
  memset(buf, ' ', spc >>= 1);
  buf[spc] = '\0';

#ifdef COLOR_HEADER
  spc = (time(0) % 7) + '1';
  prints("\033[1;4%cm�i%s�j%s\033[33m%s\033[1;37;4%cm%s�m%s�n\033[m\n",
    spc, title, buf, mid, spc, buf + len, currboard);
#else
  /* 040518.Lacool:�ק� head �C�� */
  //prints("\033[1;44m�i%s�j%s\033[33m%s\033[1;37;44m%s�m%s�n\033[m\n",
  prints("\033[1;37;44m�i%s�j%s\033[33;44m%s\033[44m%s\033[37;44m�m%s�n\033[m\n",
    title, buf, mid, buf + len, currboard);
#endif
}


/* ------------------------------------- */
/* �ʵe�B�z				 */
/* ------------------------------------- */


static char feeter[160];


/* itoc.010403: �� feeter �� status �W�ߥX�ӡA�w�ƨѨ�L function �s�� */

static void
status_foot()
{
  static int orig_flag = -1;
  static time_t uptime = -1;
  static int orig_money = -1;
  static int orig_gold = -1;
  static char flagmsg[16];
  static char coinmsg[20];

  int ufo;
  time_t now;

  ufo = cuser.ufo;
  time(&now);

  /* Thor: �P�� ��� �I�s�� �W���q�� ���� */

#ifdef HAVE_ALOHA
  ufo &= UFO_PAGER | UFO_ALOHA | UFO_CLOAK | UFO_QUIET;
  if (orig_flag != ufo)
  {
    orig_flag = ufo;
    sprintf(flagmsg,
      "%s%s%s%s",
      (ufo & UFO_PAGER) ? "��" : "�}",
      (ufo & UFO_ALOHA) ? "�W" : "  ",
      (ufo & UFO_QUIET) ? "�R" : "  ",
      (ufo & UFO_CLOAK) ? "��" : "  ");
  }
#else
  ufo &= UFO_PAGER | UFO_CLOAK | UFO_QUIET;
  if (orig_flag != ufo)
  {
    orig_flag = ufo;
    sprintf(flagmsg,
      "%s%s%s  ",
      (ufo & UFO_PAGER) ? "��" : "�}",
      (ufo & UFO_QUIET) ? "�R" : "  ",
      (ufo & UFO_CLOAK) ? "��" : "  ");
  }
#endif

  if (now > uptime)	/* �L�F�l�]�n��s�ͤ�X�� */
  {
    struct tm *ptime;

    ptime = localtime(&now);

    if (cuser.day == ptime->tm_mday && cuser.month == ptime->tm_mon + 1)
      cutmp->status |= STATUS_BIRTHDAY;
    else
      cutmp->status &= ~STATUS_BIRTHDAY;

    uptime = now + 86400 - ptime->tm_hour * 3600 - ptime->tm_min * 60 - ptime->tm_sec;
  }

  if (cuser.money != orig_money)
  {
    orig_money = cuser.money;
    sprintf(coinmsg, "��%4d%c", 
      (orig_money & 0x7FF00000) ? (orig_money >> 20) : (orig_money & 0x7FFFFC00) ? (orig_money >> 10) : orig_money, 
      (orig_money & 0x7FF00000) ? 'M' : (orig_money & 0x7FFFFC00) ? 'K' : ' ');
    coinmsg[7] = ' ';
  }
  if (cuser.gold != orig_gold)
  {
    orig_gold = cuser.gold;
    sprintf(coinmsg + 8, "��%4d%c ", 
      (orig_gold & 0x7FF00000) ? (orig_gold >> 20) : (orig_gold & 0x7FFFFC00) ? (orig_gold >> 10) : orig_gold, 
      (orig_gold & 0x7FF00000) ? 'M' : (orig_gold & 0x7FFFFC00) ? 'K' : ' ');
  }

  /* Thor.980913.����: �̱`���I�s status_foot() ���ɾ��O�C����s film�A�b 60 ��H�W�A
                       �G���ݰw�� hh:mm �ӯS�O�@�@�r���x�s�H�[�t */

  ufo = (now - (uptime - 86400)) / 60;	/* �ɥ� ufo �Ӱ��ɶ�(��) */

  /* itoc.010717: ��@�U feeter �Ϫ��שM FEETER_XXX �@�P */
  sprintf(feeter, COLOR1 " %8.8s %02d:%02d " COLOR2 " �H�� %-4d �ڬO %-12s %s [�I�s]%-9s ",
    fshm->today, ufo / 60, ufo % 60, total_user, cuser.userid, coinmsg, flagmsg);
  outf(feeter);
}


void
movie()
{
  /* Thor: it is depend on which state */

  if ((bbsmode <= M_XMENU) && (cuser.ufo & UFO_MOVIE))
    film_out(FILM_MOVIE, MENU_XNOTE);

  /* itoc.010403: �� feeter �� status �W�ߥX�� */
  status_foot();
}


typedef struct
{
  void *func;
  /* int (*func) (); */
  usint level;
  int umode;
  char *desc;
  /* wake.080601: �W�[�@�� on over string */
  char *descover;
}      MENU;


#define	MENU_LOAD	1
#define	MENU_DRAW	2
#define	MENU_FILM	4


#define	PERM_MENU	PERM_PURGE


static MENU menu_main[];


/* ----------------------------------------------------- */
/* administrator's maintain menu			 */
/* ----------------------------------------------------- */


static MENU menu_admin[] =
{
  "bin/admutil.so:a_user", PERM_ALLACCT, - M_SYSTEM,
  "User        �U�ȸ�� ",
  "User         �U�ȸ�� ",

  "bin/admutil.so:a_search", PERM_ALLACCT, - M_SYSTEM,
  "Hunt        �j�M���� ",
  "Hunt         �j�M���� ",

  "bin/admutil.so:a_editbrd", PERM_ALLBOARD, - M_SYSTEM,
  "QSetBoard   �]�w�ݪO ",
  "QSetBoard    �]�w�ݪO ",

  "bin/innbbs.so:a_innbbs", PERM_ALLBOARD, - M_SYSTEM,
  "InnBBS      ��H�]�w ",
  "InnBBS       ��H�]�w ",

#ifdef HAVE_REGISTER_FORM
  "bin/admutil.so:a_register", PERM_ALLREG, - M_SYSTEM,
  "Register    �f���U�� ",
  "Register     �f���U�� ",

  "bin/admutil.so:a_regmerge", PERM_ALLREG, - M_SYSTEM,
  "Merge       �_��f�� ",
  "Merge        �_��f�� ",
#endif

  "bin/admutil.so:a_xfile", PERM_ALLADMIN, - M_XFILES,
  "Xfile       �t���ɮ� ",
  "Xfile        �t���ɮ� ",

  "bin/admutil.so:a_restore", PERM_SYSOP, - M_SYSTEM,
  "TRestore    �٭�ƥ� ",
  "TRestore     �٭�ƥ� ",

  "bin/admutil.so:a_resetsys", PERM_ALLADMIN, - M_SYSTEM,
  "BBSreset    ���m�t�� ",
  "BBSreset     ���m�t�� ",

  menu_main, PERM_MENU + Ctrl('A'), M_AMENU,
  "�t�κ��@"
};


/* ----------------------------------------------------- */
/* mail menu						 */
/* ----------------------------------------------------- */


static int
XoMbox()
{
  xover(XZ_MBOX);
  return 0;
}


static MENU menu_mail[] =
{
  XoMbox, PERM_BASIC, M_RMAIL,
  "Read       �A���W�ūH�c",
  "Read        �A���W�ūH�c",

  m_send, PERM_LOCAL, M_SMAIL,
  "Mail       �H�������u���@�H",
  "Mail        �H�������u���@�H",

#ifdef MULTI_MAIL  /* Thor.981009: ����R�����B�H */
  m_list, PERM_LOCAL, M_SMAIL,
  "List       �H�������n�h�H",
  "List        �H�������n�h�H",
#endif

  m_internet, PERM_INTERNET, M_SMAIL,
  "Internet   �H�쯸�~�q�o�H�c",
  "Internet    �H�쯸�~�q�o�H�c",

#ifdef HAVE_SIGNED_MAIL
  m_verify, 0, M_XMODE,
  "Verify     ����u��",
  "Verify      ����u��",
#endif

#ifdef HAVE_MAIL_ZIP
  m_zip, PERM_INTERNET, M_SMAIL,
  "Zip        ���]�~�a�]�N�u�u�^",
  "Zip         ���]�~�a�]�N�u�u�^",
#endif

  m_sysop, 0, M_SMAIL,
  "Yes Sir!   �]�b���Z����",
  "Yes  Sir!   �]�b���Z����",
  
  "bin/admutil.so:m_bm", PERM_ALLADMIN, - M_SMAIL,
  "BM All     ���Z�����D",	/* itoc.000512: �s�W m_bm */
  "BM All      ���Z�����D",

  "bin/admutil.so:m_all", PERM_ALLADMIN, - M_SMAIL,
  "User All   ���Z�Ҧ��H",	/* itoc.000512: �s�W m_all */
  "User All    ���Z�Ҧ��H",

  menu_main, PERM_MENU + Ctrl('A'), M_MMENU,	/* itoc.020829: �� guest �S�ﶵ */
  "�q�l�l��"
};


/* ----------------------------------------------------- */
/* talk menu						 */
/* ----------------------------------------------------- */


static int
XoUlist()
{
  xover(XZ_ULIST);
  return 0;
}


static MENU menu_talk[];


  /* --------------------------------------------------- */
  /* list menu						 */
  /* --------------------------------------------------- */

static MENU menu_list[] =
{
  t_pal, PERM_BASIC, M_PAL,
  "Pal        �n�ͦW��",
  "Pal         �n�ͦW��",

#ifdef HAVE_LIST
  t_list, PERM_BASIC, M_PAL,
  "List       �����ƱƯ�",
  "List        �����ƱƯ�",
#endif

#ifdef HAVE_ALOHA
  "bin/aloha.so:t_aloha", PERM_PAGE, - M_PAL,
  "Aloha      �W�����i",
  "Aloha       �W�����i",
#endif

#ifdef LOGIN_NOTIFY
  t_loginNotify, PERM_PAGE, M_PAL,
  "Notify     �t�Ψ�M",
  "Notify      �t�Ψ�M",
#endif

  menu_talk, PERM_MENU + 'P', M_TMENU,
  "�U���W��"
};


static MENU menu_talk[] =
{
  XoUlist, 0, M_LUSERS,
  "Users      ���W�ϥΪ̦C��",
  "Users       ���W�ϥΪ̦C��",

  menu_list, PERM_BASIC, M_TMENU,
  "ListMenu   �U���W��]�w",
  "ListMenu    �U���W��]�w",

  t_pager, PERM_BASIC, M_XMODE,
  "Pager      �����I�s���A",
  "Pager       �����I�s���A",

  t_cloak, PERM_CLOAK, M_XMODE,
  "Invis      �ϥ������K��",
  "Invis       �ϥ������K��",

  t_query, 0, M_QUERY,
  "Query      �������q����",
  "Query       �������q����",

  t_talk, PERM_PAGE, M_PAGE,
  "Talk       ��H���",
  "Talk        ��H���",

  /* Thor.990220: ��ĥ~�� */
  "bin/chat.so:t_chat", PERM_CHAT, - M_CHAT,
  "ChatRoom   �ԩ����ԫȩ�",
  "ChatRoom    �ԩ����ԫȩ�",

  t_display, PERM_BASIC, M_BMW,
  "Display    �s�����y�T��",
  "Display     �s�����y�T��",

  t_bmw, PERM_BASIC, M_BMW,
  "Write      �^�U���y�O��",
  "Write       �^�U���y�O��",

  menu_main, PERM_MENU + 'U', M_TMENU,
  "�𶢲��"
};


/* ----------------------------------------------------- */
/* user menu						 */
/* ----------------------------------------------------- */


static MENU menu_user[];


  /* --------------------------------------------------- */
  /* register menu                                      */
  /* --------------------------------------------------- */

static MENU menu_register[] =
{
  u_addr, PERM_BASIC, M_XMODE,
  "Address    �]�w�q�l�H�c",
  "Address     �]�w�q�l�H�c",

#ifdef HAVE_REGISTER_FORM
  u_register, PERM_BASIC, M_UFILES,
  "Register   ��g���U��",
  "Register    ��g���U��",
#endif

#ifdef HAVE_REGKEY_CHECK
  u_verify, PERM_BASIC, M_UFILES,
  "Verify     ��g�{�ҽX",
  "Verify      ��g�{�ҽX",
#endif

  u_deny, PERM_BASIC, M_XMODE,
  "Perm       ��_�v��",
  "Perm        ��_�v��",

  menu_user, PERM_MENU + 'A', M_UMENU,
  "���U���"
};


static MENU menu_user[] =
{
  u_info, PERM_BASIC, M_XMODE,
  "Info       �s�׭ӤH���",
  "Info        �s�׭ӤH���",

  u_setup, 0, M_UFILES,
  "Habit      �]�w�ߦn�Ҧ�",
  "Habit       �]�w�ߦn�Ҧ�",

  menu_register, PERM_BASIC, M_UMENU,
  "Register   ��g���U��",
  "Register    ��g���U��",

  pad_view, 0, M_READA,
  "Note       �[�ݪ��ͯd��",
  "Note        �[�ݪ��ͯd��",

  /* itoc.010309: ���������i�H�g�d���O */
  pad_draw, PERM_POST, M_POST,
  "Pad        ��~�d����",
  "Pad         ��~�d����",

  u_lock, PERM_BASIC, M_IDLE,
  "Lock       ��w�ù��e��",
  "Lock        ��w�ù��e��",

  u_xfile, PERM_BASIC, M_UFILES,
  "Xfile      �s�׭ӤH�ɮ�",
  "Xfile       �s�׭ӤH�ɮ�",

  u_log, PERM_BASIC, M_UFILES,
  "ViewLog    �ӤH�W���O��",
  "ViewLog     �ӤH�W���O��",

  menu_main, PERM_MENU + 'H', M_UMENU,
  "�ӤH�]�w"
};


#ifdef HAVE_EXTERNAL

/* ----------------------------------------------------- */
/* tool menu						 */
/* ----------------------------------------------------- */


static MENU menu_tool[];


#ifdef HAVE_SONG
  /* --------------------------------------------------- */
  /* song menu						 */
  /* --------------------------------------------------- */

static MENU menu_song[] =
{
  "bin/song.so:XoSongLog", 0, - M_XMODE,
  "KTV        �I�q�O��ï",
  "KTV         �I�q�O��ï",

  "bin/song.so:XoSongMain", 0, - M_XMODE,
  "Book       ���}�I�۾�",
  "Book        ���}�I�۾�",

  "bin/song.so:XoSongSub", 0, - M_XMODE,
  "Note       ��Z�s�q��",
  "Note        ��Z�s�q��",

  menu_tool, PERM_MENU + 'K', M_XMENU,
  "���I�q��"
};
#endif


#ifdef HAVE_NETTOOL
  /* --------------------------------------------------- */
  /* net menu                                            */
  /* --------------------------------------------------- */

static MENU menu_net[] =
{
  /* itoc.010821: �@��²�檺�A�ȡA�� guest �]�i�H�ΡF�S���A�Ȥ~�n�D PERM_INTERNET */

  "bin/enews.so:main_enews", 0, - M_XMODE,
  "Kimo News  ����_���s�D",
  "Kimo News   ����_���s�D",

  "bin/railway.so:main_railway", 0, - M_XMODE,
  "Railway    �x�K�ɨ��",
  "Railway     �x�K�ɨ��",

  "bin/bbsnet.so:x_bbsnet", PERM_INTERNET, - M_XMODE,
  "BBSnet     �i�J�ɪŤ���",
  "BBSnet      �i�J�ɪŤ���",

  menu_tool, PERM_MENU + 'K', M_XMENU,
  "�����A��"
};
#endif


#ifdef HAVE_GAME

#if 0

  itoc.010426.����:
  �q���C�����ν����סA�����a���n�����A�u�[���A������C

  itoc.010714.����:
  (a) �C�����C�����`��������b 1.01�A�@�ӱߤW���i�� 100 ���C���A
      �Y�N�`�]����J�h���C���A�h 1.01^100 = 2.7 ��/�C���@�ӱߤW�C
  (b) �Y�U�����v�������A�]�������b 1.0 ~ 1.02 �����A�����a�@�w���ȿ��A
      �B�Y�@����̰�����Ȫ����@���A�]���|�ȱo�L�����СC
  (c) ��h�W�A���v�V�C�̨��������� 1.02�A���v�����̨��������� 1.01�C

  itoc.011011.����:
  ���F�קK user multi-login ���Ӭ~���A
  �ҥH�b���C�����}�l�N�n�ˬd�O�_���� login �Y if (HAS_STATUS(STATUS_COINLOCK))�C

#endif

  /* --------------------------------------------------- */
  /* game menu						 */
  /* --------------------------------------------------- */

static MENU menu_game[];

static MENU menu_game1[] =
{
  "bin/liteon.so:main_liteon", 0, - M_GAME,
  "0LightOn    �ж��}�O ",
  "0LightOn     �ж��}�O ",

  "bin/guessnum.so:guessNum", 0, - M_GAME,
  "1GuessNum   ���q�Ʀr ",
  "1GuessNum    ���q�Ʀr ",

  "bin/guessnum.so:fightNum", 0, - M_GAME,
  "2FightNum   ���q�Ʀr ",
  "2FightNum    ���q�Ʀr ",

  "bin/km.so:main_km", 0, - M_GAME,
  "3KongMing   �թ����� ",
  "3KongMing    �թ����� ",

  "bin/recall.so:main_recall", 0, - M_GAME,
  "4Recall     �^�Ф��Z ",
  "4Recall      �^�Ф��Z ",

  "bin/mine.so:main_mine", 0, - M_GAME,
  "5Mine       �ý�a�p ",
  "5Mine        �ý�a�p ",

  "bin/fantan.so:main_fantan", 0, - M_GAME, 
  "6Fantan     �f�u���s ",
  "6Fantan      �f�u���s ",

  "bin/dragon.so:main_dragon", 0, - M_GAME,
  "7Dragon     ���s�C�� ",
  "7Dragon      ���s�C�� ",

  "bin/nine.so:main_nine", 0, - M_GAME,
  "8Nine       �Ѧa�E�E ",
  "8Nine        �Ѧa�E�E ",

  menu_game, PERM_MENU + '0', M_XMENU,
  "�q���Ŷ�"
};

static MENU menu_game2[] =
{
  "bin/dice.so:main_dice", 0, - M_GAME,
  "0Dice       �g�Y��l ",
  "0Dice        �g�Y��l ",

  "bin/gp.so:main_gp", 0, - M_GAME,
  "1GoldPoker  ���P���J ",
  "1GoldPoker   ���P���J ",

  "bin/bj.so:main_bj", 0, - M_GAME,
  "2BlackJack  �G�Q�@�I ",
  "2BlackJack   �G�Q�@�I ",

  "bin/chessmj.so:main_chessmj", 0, - M_GAME,
  "3ChessMJ    �H�ѳ±N ",
  "3ChessMJ     �H�ѳ±N ",

  "bin/seven.so:main_seven", 0, - M_GAME,
  "4Seven      �䫰�C�i ",
  "4Seven       �䫰�C�i ",
 
  "bin/race.so:main_race", 0, - M_GAME,
  "5Race       �i�ɰ��� ",
  "5Race        �i�ɰ��� ",

  "bin/bingo.so:main_bingo", 0, - M_GAME,
  "6Bingo      ���G�j�� ",
  "6Bingo       ���G�j�� ",

  "bin/marie.so:main_marie", 0, - M_GAME,
  "7Marie      �j�p���� ",
  "7Marie       �j�p���� ",

  "bin/bar.so:main_bar", 0, - M_GAME,
  "8Bar        �a�x���� ",
  "8Bar         �a�x���� ",

  menu_game, PERM_MENU + '0', M_XMENU,
  "�C���ֶ�"
};

static MENU menu_game3[] =
{
  "bin/pip.so:main_pip", PERM_BASIC, - M_GAME,
  "0Chicken    �q�l�p�� ",
  "0Chicken     �q�l�p�� ",

  "bin/pushbox.so:main_pushbox", 0, - M_GAME,
  "1PushBox    �ܮw�f�f ",
  "1PushBox     �ܮw�f�f ",

  "bin/tetris.so:main_tetris", 0, - M_GAME,
  "2Tetris     �Xù���� ",
  "2Tetris      �Xù���� ",

  "bin/reversi.so:main_reversi", 0, - M_GAME,
  "3Reversi    �L�Ǥj�� ",
  "3Reversi     �L�Ǥj�� ",

  menu_game, PERM_MENU + '0', M_XMENU,
  "�Ϥ�S��"
};

static MENU menu_game[] =
{
  menu_game1, PERM_BASIC, M_XMENU,
  "1Game      �i �q���Ѱ� �j",
  "1Game       �i �q���Ѱ� �j",

  menu_game2, PERM_BASIC, M_XMENU,
  "2Game      �i �C���ֶ� �j",
  "2Game       �i �C���ֶ� �j",

  menu_game3, PERM_BASIC, M_XMENU,
  "3Game      �i �Ϥ�S�� �j",
  "3Game       �i �Ϥ�S�� �j",

  menu_tool, PERM_MENU + '1', M_XMENU,
  "�C���H��"
};
#endif


#ifdef HAVE_BUY
  /* --------------------------------------------------- */
  /* buy menu						 */
  /* --------------------------------------------------- */

static MENU menu_buy[] =
{
  "bin/bank.so:x_bank", PERM_BASIC, - M_GAME,
  "Bank       �ԩ��H�U�Ȧ� ",
  "Bank        �ԩ��H�U�Ȧ� ",

  "bin/bank.so:b_invis", PERM_BASIC, - M_GAME,
  "Invis      ���β{�� ",
  "Invis       ���β{�� ",

  "bin/bank.so:b_cloak", PERM_BASIC, - M_GAME,
  "Cloak      �L������ ",
  "Cloak       �L������ ",

  "bin/bank.so:b_mbox", PERM_BASIC, - M_GAME,
  "Mbox       �H�c�L�W�� ",
  "Mbox        �H�c�L�W�� ",

  "bin/bank.so:b_xempt", PERM_BASIC, - M_GAME,
  "Xempt      �b���ä[�O�d ",
  "Xempt       �b���ä[�O�d ",

  menu_tool, PERM_MENU + 'B', M_XMENU,
  "���ĥ���"
};
#endif


  /* --------------------------------------------------- */
  /* other tools menu					 */
  /* --------------------------------------------------- */

static MENU menu_other[] =
{
  "bin/vote.so:vote_all", PERM_BASIC, - M_VOTE,	/* itoc.010414: �벼���� */
  "VoteAll     �벼���� ",
  "VoteAll      �벼���� ",

#ifdef HAVE_TIP
  "bin/xyz.so:x_tip", 0, - M_READA,
  "Tip         �оǺ��F ",
  "Tip          �оǺ��F ",
#endif

#ifdef HAVE_LOVELETTER
  "bin/xyz.so:x_loveletter", 0, - M_READA,
  "LoveLetter  ���Ѽ��g ",
  "LoveLetter   ���Ѽ��g ",
#endif

  "bin/xyz.so:x_password", PERM_VALID, - M_XMODE,
  "Password    �ѰO�K�X ",
  "Password     �ѰO�K�X ",

#ifdef HAVE_CLASSTABLE
  "bin/classtable.so:main_classtable", PERM_BASIC, - M_XMODE,
  "ClassTable  �\\�Үɬq ",
  "ClassTable   �\\�Үɬq ",
#endif

#ifdef HAVE_CREDIT
  "bin/credit.so:main_credit", PERM_BASIC, - M_XMODE,
  "MoneyNote   �O�b�⥾ ",
  "MoneyNote    �O�b�⥾ ",
#endif

#ifdef HAVE_CALENDAR
  "bin/todo.so:main_todo", PERM_BASIC, - M_XMODE,
  "XTodo       �ӤH��{ ",
  "XTodo        �ӤH��{ ",

  "bin/calendar.so:main_calendar", 0, - M_XMODE,
  "YCalendar   �U�~��� ",
  "YCalendar    �U�~��� ",
#endif

  menu_tool, PERM_MENU + Ctrl('A'), M_XMENU,	/* itoc.020829: �� guest �S�ﶵ */
  "��L�\\��"
};


static MENU menu_tool[] =
{
#ifdef HAVE_NETTOOL
  menu_net, 0, M_XMENU, /* itoc.010821: �@��²�檺�A�ȡA�� guest �]�i�H�ΡF�S���A�Ȥ~�n�D PERM_INTERNET */
  "Net        �i �����A�� �j",
  "Net         �i �����A�� �j",
#endif

#ifdef HAVE_SONG
  menu_song, 0, M_XMENU,
  "KTV        �i �u���I�q �j",
  "KTV         �i �u���I�q �j",
#endif

#ifdef HAVE_COSIGN
  /* 041022.Lacool:�ޱ����h���s�p */
  /*
  "bin/newbrd.so:XoNewBoard", PERM_VALID, - M_XMODE,
  "Join       �i �ݪO�s�p �j",
  "Join        �i �ݪO�s�p �j",
  */
#endif

#ifdef HAVE_GAME
  menu_game, PERM_BASIC, M_XMENU,
  "Game       �i �C���H�� �j",
  "Game        �i �C���H�� �j",
#endif

#ifdef HAVE_BUY
  menu_buy, PERM_BASIC, M_XMENU,
  "Market     �i ���ĥ��� �j",
  "Market      �i ���ĥ��� �j",
#endif

  menu_other, 0, M_XMENU,
  "Other      �i ���C���K �j",
  "Other       �i ���C���K �j",

  menu_main, PERM_MENU + Ctrl('A'), M_XMENU,	/* itoc.020829: �� guest �S�ﶵ */
  "�ӤH�u��"
};

#endif	/* HAVE_EXTERNAL */


/* ----------------------------------------------------- */
/* main menu						 */
/* ----------------------------------------------------- */


static int
Gem()
{
  /* itoc.001109: �ݪO�`�ަb (A)nnounce �U�� GEM_X_BIT�A��K�}�O */
  XoGem("gem/"FN_DIR, "��اG�i��", (HAS_PERM(PERM_ALLBOARD) ? (GEM_W_BIT | GEM_X_BIT | GEM_M_BIT) : 0));
  return 0;
}


static MENU menu_main[] =
{
  /* 041023.Lacool:���èt�οﶵ */
#if 0
  menu_admin, PERM_ALLADMIN, M_AMENU,
  "0Admin    �i �t�κ��@�� �j",
  "0Admin     �i �t�κ��@�� �j",
#endif

  Gem, 0, M_GEM,
  "Announce  �i ��ؤ��G�� �j",
  "Announce   �i ��ؤ��G�� �j",

  Boards, 0, M_BOARD,
  "Boards    �i �G�i�Q�װ� �j",
  "Boards     �i �G�i�Q�װ� �j",

  Class, 0, M_BOARD,
  "Class     �i ���հQ�׶� �j",
  "Class      �i ���հQ�׶� �j",

#ifdef MY_FAVORITE
  MyFavorite, PERM_BASIC, M_MF,
  "Favorite  �i �ڪ��̷R�s �j",
  "Favorite   �i �ڪ��̷R�s �j",
#endif

  menu_mail, 0, M_MMENU, 
  "Mail      �i �H����ò� �j",
  "Mail       �i �H����ò� �j",

  menu_talk, 0, M_TMENU,
  "Talk      �i �𶢲�Ѧa �j",
  "Talk       �i �𶢲�Ѧa �j",

  menu_user, 0, M_UMENU,
  "User      �i �ӤH�u��{ �j",
  "User       �i �ӤH�u��{ �j",

#ifdef HAVE_EXTERNAL
  menu_tool, 0, M_XMENU,
  "Xyz       �i �S��۫ݩ� �j",
  "Xyz        �i �S��۫ݩ� �j",
#endif

#if 0	/* itoc.010209: ���� s �����i�J Select() ��ֿ����� */
  Select, 0, M_BOARD,
  "Select    �i ��ܥD�ݪO �j",
  "Select     �i ��ܥD�ݪO �j",
#endif

  goodbye, 0, M_XMODE,
  "Goodbye   �i �U���A�|�a �j",
  "Goodbye    �i �U���A�|�a �j",

  NULL, PERM_MENU + 'B', M_0MENU,
  "�D�\\���"
};


void
menu()
{
  MENU *menu, *mptr, *table[12];
  usint level, mode;
  int cc, cx;			/* current / previous cursor position */
  int max, mmx;			/* current / previous menu max */
  int cmd, depth;
  char *str;
  /* 040518.Lacool:�W�[�����Χ����׹��� */
  char graphics[6][29]=
      {"  ( �� c��)y�c�i            ",
       "( ������)�w�w�wC�բw___-)|||",
       "  (��������)��~ ��������    ",
       "  (�ã`(#��) #��=(�@-�@o)   ",
       "  ��(���㡫\")��            ",
       " <(�á`��)>  a(����áF)    "};

  int graphics_show = time(0) % 6 ;
  char graphics_clear[29]={"                            "};

  mode = MENU_LOAD | MENU_DRAW | MENU_FILM;
  menu = menu_main;
  level = cuser.userlevel;
  depth = mmx = 0;

  for (;;)
  {
    if (mode & MENU_LOAD)
    {
      for (max = -1;; menu++)
      {
	cc = menu->level;
	if (cc & PERM_MENU)
	{

#ifdef	MENU_VERBOSE
	  if (max < 0)		/* �䤣��A�X�v�����\��A�^�W�@�h�\��� */
	  {
	    menu = (MENU *) menu->func;
	    continue;
	  }
#endif

	  break;
	}
	if (cc && !(cc & level))	/* �����v���~�q�X */
	  continue;

	table[++max] = menu;
      }

      if (mmx < max)
	mmx = max;

      if ((depth == 0) && HAS_STATUS(STATUS_BIFF))	/* �Ĥ@���W���Y���s�H�A�i�J Mail ��� */
	cmd = 'M';
      else
	cmd = cc ^ PERM_MENU;	/* default command */
      utmp_mode(menu->umode);
    }

    if (mode & MENU_DRAW)
    {
      if (mode & MENU_FILM)
      {
	clear();
	movie();
	cx = -1;
      }

      vs_head(menu->desc, NULL);

      mode = 0;
      do
      {
	move(MENU_XPOS + mode, MENU_YPOS + 2);
	if (mode <= max)
	{
	  mptr = table[mode];
	  str = mptr->desc;
	  prints("(\033[1;36m%c\033[m)", *str++);
	  outs(str);
	}
	clrtoeol();
      } while (++mode <= mmx);

      mmx = max;
      mode = 0;
    }

    switch (cmd)
    {
    case KEY_DOWN:
      cc = (cc == max) ? 0 : cc + 1;
      break;

    case KEY_UP:
      cc = (cc == 0) ? max : cc - 1;
      break;

    case Ctrl('A'):	/* itoc.020829: �w�]�ﶵ�Ĥ@�� */
    case KEY_HOME:
      cc = 0;
      break;

    case KEY_END:
      cc = max;
      break;

    case KEY_PGUP:
      cc = (cc == 0) ? max : 0;
      break;

    case KEY_PGDN:
      cc = (cc == max) ? 0 : max;
      break;

    case '\n':
    case KEY_RIGHT:
      mptr = table[cc];
      cmd = mptr->umode;
#if 1
     /* Thor.990212: dynamic load , with negative umode */
      if (cmd < 0)
      {
	void *p = DL_get(mptr->func);
	if (!p)
	  break;
	mptr->func = p;
	cmd = -cmd;
	mptr->umode = cmd;
      }
#endif
      utmp_mode(cmd);

      if (cmd <= M_XMENU)	/* �l�ؿ��� mode �n <= M_XMENU */
      {
	menu->level = PERM_MENU + mptr->desc[0];
	menu = (MENU *) mptr->func;

	mode = MENU_LOAD | MENU_DRAW;
	/* mode = MENU_LOAD | MENU_DRAW | MENU_FILM;	/* itoc.010304: �i�J�l��歫�� movie */

	depth++;
	continue;
      }

      {
	int (*func) ();

	func = mptr->func;
	mode = (*func) ();
      }

      utmp_mode(menu->umode);

      if (mode == XEASY)
      {
	outf(feeter);
	mode = 0;
      }
      else
      {
	mode = MENU_DRAW | MENU_FILM;
      }

      cmd = mptr->desc[0];
      continue;

#ifdef EVERY_Z
    case Ctrl('Z'):
      every_Z(0);
      goto every_key;

    case Ctrl('U'):
      every_U(0);
      goto every_key;
#endif

    /* itoc.010911: Select everywhere�A���A����O�b M_0MENU */
    case 's':
    case Ctrl('S'):
      utmp_mode(M_BOARD);
      Select();
      goto every_key;

   /* 041023.Lacool:�ק�D����'0'�i�J�t�κ޲z��� */
   case '0':
     if (bbsmode == M_0MENU)
     {
       if (HAS_PERM(PERM_ALLADMIN))
       {
         utmp_mode(M_AMENU);
         menu->level = PERM_MENU + table[cc]->desc[0];
         menu = (MENU *) menu_admin;
         mode = MENU_LOAD | MENU_DRAW;
         depth++;
         continue;
       }
       goto every_key;
     }
     goto default_key; /* �Y���b M_0MENU ���� r ���ܡA�n�����@����� */

#ifdef MY_FAVORITE
    /* itoc.010911: Favorite everywhere�A���A����O�b M_0MENU */
    /* 041103.Lacool:�ޱ��� f �����i�J�ڪ��̷R */
    //case 'f':
    case Ctrl('F'):
      if (cuser.userlevel)	/* itoc.010407: �n�ˬd�v�� */
      {
	utmp_mode(M_MF);
	MyFavorite();
      }
      goto every_key;
#endif

    /* itoc.020301: Read currboard in M_0MENU */
    case 'r':
      if (bbsmode == M_0MENU)
      {
	if (currbno >= 0)
	{
	  utmp_mode(M_BOARD);
	  XoPost(currbno);
	  xover(XZ_POST);
#ifndef ENHANCED_VISIT
	  time(&brd_visit[currbno]);
#endif
	}
	goto every_key;
      }
      goto default_key;	/* �Y���b M_0MENU ���� r ���ܡA�n�����@����� */

every_key:	/* �S����B�z���� */
      utmp_mode(menu->umode);
      mode = MENU_DRAW | MENU_FILM;
      cmd = table[cc]->desc[0];
      continue;

    case KEY_LEFT:
    case 'e':
      if (depth > 0)
      {
	menu->level = PERM_MENU + table[cc]->desc[0];
	menu = (MENU *) menu->func;
	mode = MENU_LOAD | MENU_DRAW;
	/* mode = MENU_LOAD | MENU_DRAW | MENU_FILM;	/* itoc.010304: �h�X�l��歫�� movie */
	depth--;
	continue;
      }
      cmd = 'G';

default_key:
    default:

      if (cmd >= 'a' && cmd <= 'z')
	cmd ^= 0x20;			/* �ܤj�g */

      cc = 0;
      for (;;)
      {
	if (table[cc]->desc[0] == cmd)
	  break;
	if (++cc > max)
	{
	  cc = cx;
	  goto menu_key;
	}
      }
    }

    if (cc != cx)	/* �Y��в��ʦ�m */
    {
#ifdef CURSOR_BAR
      if (cx >= 0)
      {
	move(MENU_XPOS + cx, MENU_YPOS);
	if (cx <= max)
	{
	  mptr = table[cx];
	  str = mptr->desc;
	  /* 040518.Lacool:�קאּ�F���Ϋ�ݭ׹��ϡA�G�C���n�h�M�� */
	  prints("  (\033[1;36m%c\033[m)%s %s", *str, str + 1, graphics_clear);
	}
	else
	{
	  outs("  ");
	}
      }
      move(MENU_XPOS + cc, MENU_YPOS);
      mptr = table[cc];
      /* wake.080601: �H menu ���W�[�� on over string ����X�A�ÿ�X���Ϋ�ݭ׹��� */
      str = mptr->descover;
      prints(" +" COLOR4 " \033[1;33m%s\033[m %s", str, graphics[graphics_show]);
	  /* Old code
      str = mptr->desc;
	  prints(COLOR4 "> (%c)%s \033[m", *str, str + 1);
	  */
      cx = cc;
#else		/* �S�� CURSOR_BAR */
      if (cx >= 0)
      {
	move(MENU_XPOS + cx, MENU_YPOS);
	outc(' ');
      }
      move(MENU_XPOS + cc, MENU_YPOS);
      outc('>');
      cx = cc;
#endif
    }
    else		/* �Y��Ъ���m�S���� */
    {
#ifdef CURSOR_BAR
      move(MENU_XPOS + cc, MENU_YPOS);
      mptr = table[cc];
      /* wake.080601: �H menu ���W�[�� on over string ����X�A�ÿ�X���Ϋ�ݭ׹��� */
      str = mptr->descover;
      prints(" +" COLOR4 " \033[1;33m%s\033[m %s", str, graphics[graphics_show]);
	  /* Old code
      str = mptr->descr;
      prints(COLOR4 "> (%c)%s \033[m", *str, str + 1);
	  */
#else
      move(MENU_XPOS + cc, MENU_YPOS + 1);
#endif
    }

menu_key:

    cmd = vkey();
  }
}

