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


#define BRD_NOZAP	0x0001	/* 不可 zap */
#define BRD_NOTRAN	0x0002	/* 不轉信 */
#define BRD_NOCOUNT	0x0004	/* 不計文章發表篇數 */
#define BRD_NOSTAT	0x0008	/* 不納入熱門話題統計 */
#define BRD_NOVOTE	0x0010	/* 不公佈投票結果於 [record] 板 */
#define BRD_ANONYMOUS	0x0020	/* 匿名看板 */

/* 041115.Lacool: 增加屬性 */
#ifdef HAVE_SCORE
  #define BRD_NOSCORE	0x0040	/* 看板不可推文 */
#endif
#ifdef HAVE_EPOST
  #define BRD_NOEPOST	0x0080	/* 看板不可接文 */
#endif

/* 041130.Lacool: 增加屬性 */
#ifdef HAVE_POSTLAW
  #define BRD_POSTLAW	0x0100	/* 看板使用自訂發文綱領 */
#endif
#ifdef POST_PREFIX
  #define BRD_PREFIX 0x0200  /* 看板使用發文類別 */
#endif
#ifdef PREFIX_EDIT
  #define BRD_PREFIX_EDIT 0x0400  /* 看板自訂發文類別 */
#endif


/* ----------------------------------------------------- */
/* 各種旗標的中文意義					 */
/* ----------------------------------------------------- */

/* 041115.Lacool: 增加屬性 */
#define NUMBATTRS	12

#define STR_BATTR	"zTcsvA%xeLCb"			/* itoc: 新增旗標的時候別忘了改這裡啊 */


#ifdef _ADMIN_C_
static char *battr_tbl[NUMBATTRS] =
{
  "不可 Zap",			/* BRD_NOZAP */
  "不轉信出去",			/* BRD_NOTRAN */
  "不記錄篇數",			/* BRD_NOCOUNT */
  "不做熱門話題統計",		/* BRD_NOSTAT */
  "不公開投票結果",		/* BRD_NOVOTE */
  "匿名看板",			/* BRD_ANONYMOUS */
  "不評分看板",			/* BRD_NOSCORE */

/* 041115.Lacool: 增加屬性 */
#ifdef HAVE_SCORE
  "不可推文",                   /* BRD_NOSCORE */
#endif

#ifdef HAVE_EPOST
  "不可接文",                   /* BRD_NOEPOST */
#endif

/* 041130.Lacool: 增加自訂發文剛領屬性 */
#ifdef HAVE_POSTLAW
  "使用自訂發文綱領",
#endif

/* 041130.Lacool: 看板自訂發文類別 */
#ifdef POST_PREFIX
  "使用發文類別",
#endif

#ifdef PREFIX_EDIT
        "發文類別自訂/預設",
#endif

};

#endif

#endif				/* _BATTR_H_ */
