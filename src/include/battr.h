/*-------------------------------------------------------*/
/* battr.h	( NTHU CS MapleBBS Ver 2.36 )		 */
/*-------------------------------------------------------*/
/* target : Board Attribution				 */
/* create : 95/03/29				 	 */
/* update : 95/12/15				 	 */
/*-------------------------------------------------------*/


#ifndef	_BATTR_H_
#define	_BATTR_H_


/* ----------------------------------------------------- */
/* Board Attribution : flags in BRD.battr		 */
/* ----------------------------------------------------- */


#define BRD_NOZAP	0x0001	/* ���i zap */
#define BRD_NOTRAN	0x0002	/* ����H */
#define BRD_NOCOUNT	0x0004	/* ���p�峹�o��g�� */
#define BRD_NOSTAT	0x0008	/* ���ǤJ�������D�έp */
#define BRD_NOVOTE	0x0010	/* �����G�벼���G�� [record] �O */
#define BRD_ANONYMOUS	0x0020	/* �ΦW�ݪO */

/* 041115.Lacool: �W�[�ݩ� */
#ifdef HAVE_SCORE
  #define BRD_NOSCORE	0x0040	/* �ݪO���i���� */
#endif
#ifdef HAVE_EPOST
  #define BRD_NOEPOST	0x0080	/* �ݪO���i���� */
#endif

/* 041130.Lacool: �W�[�ݩ� */
#ifdef HAVE_POSTLAW
  #define BRD_POSTLAW	0x0100	/* �ݪO�ϥΦۭq�o����� */
#endif
#ifdef POST_PREFIX
  #define BRD_PREFIX 0x0200  /* �ݪO�ϥεo�����O */
#endif
#ifdef PREFIX_EDIT
  #define BRD_PREFIX_EDIT 0x0400  /* �ݪO�ۭq�o�����O */
#endif


/* ----------------------------------------------------- */
/* �U�غX�Ъ�����N�q					 */
/* ----------------------------------------------------- */

/* 041115.Lacool: �W�[�ݩ� */
#define NUMBATTRS	12

#define STR_BATTR	"zTcsvA%xeLCb"			/* itoc: �s�W�X�Ъ��ɭԧO�ѤF��o�̰� */


#ifdef _ADMIN_C_
static char *battr_tbl[NUMBATTRS] =
{
  "���i Zap",			/* BRD_NOZAP */
  "����H�X�h",			/* BRD_NOTRAN */
  "���O���g��",			/* BRD_NOCOUNT */
  "�����������D�έp",		/* BRD_NOSTAT */
  "�����}�벼���G",		/* BRD_NOVOTE */
  "�ΦW�ݪO",			/* BRD_ANONYMOUS */
  "�������ݪO",			/* BRD_NOSCORE */

/* 041115.Lacool: �W�[�ݩ� */
#ifdef HAVE_SCORE
  "���i����",                   /* BRD_NOSCORE */
#endif

#ifdef HAVE_EPOST
  "���i����",                   /* BRD_NOEPOST */
#endif

/* 041130.Lacool: �W�[�ۭq�o�����ݩ� */
#ifdef HAVE_POSTLAW
  "�ϥΦۭq�o�����",
#endif

/* 041130.Lacool: �ݪO�ۭq�o�����O */
#ifdef POST_PREFIX
  "�ϥεo�����O",
#endif

#ifdef PREFIX_EDIT
        "�o�����O�ۭq/�w�]",
#endif

};

#endif

#endif				/* _BATTR_H_ */
