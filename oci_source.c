#include"oci_source.h"
int  CheckErr(OCIError* err, sword status){
	text errbuf[512];
	ub4 buflen;
	ub4 errorcode;

	switch (status){
		case OCI_SUCCESS:
			break;
		case OCI_SUCCESS_WITH_INFO:
	                printf("Error - OCI_SUCCESS_WITH_INFO\n");
			break;
		case OCI_NEED_DATA:
		        printf("ERROR - OCI_NEED_DATA\n");
			break;
		case OCI_NO_DATA:
			printf("error - OCI_NO_DATA\n");
			break;
		case OCI_ERROR:
			OCIErrorGet(err,(ub4) 1,(text *) NULL, &errorcode, errbuf, (ub4) sizeof(errbuf), (ub4) OCI_HTYPE_ERROR);
		case OCI_INVALID_HANDLE:
			printf("error - OCI_INVALID_HANDLE\n");
			break;
		case OCI_STILL_EXECUTING:
			printf("error - OCI_STILL_EXECUTING\n");
			break;
		case OCI_CONTINUE:
			printf("error - OCI_CONTINUE\n");
			break;
		default:
			break;
	}
}
int  Del_Source(Ocihandle *handle, Source *source){
	char sqltext[1024];
	sprintf(sqltext,"DELETE FROM URL_SOURCE WHERE ID = '%s' ",source->ID);
	CheckErr(handle->errhp, OCIStmtPrepare(handle->stmthp, handle->errhp, sqltext, strlen((char*)sqltext), OCI_NTV_SYNTAX, OCI_DEFAULT));
	CheckErr(handle->errhp, OCIStmtExecute(handle->svchp, handle->stmthp, handle->errhp, 1, 0, NULL, NULL, OCI_DEFAULT));
	CheckErr(handle->errhp, OCITransCommit(handle->svchp, handle->errhp, 0));
	int rows_processed = 0;
	OCIAttrGet(handle->stmthp, OCI_HTYPE_STMT, &rows_processed, 0, OCI_ATTR_ROW_COUNT, handle->errhp);
	printf("已经成功删除%d行\n",rows_processed);
	return 0;
}
int Alt_Source(Ocihandle *handle, Source *source){
	char sqltext[1024];
	sprintf(sqltext, "UPDATE URL_SOURCE SET URL_SOURCE = '%s' WHERE ID = '%s'",source->URL_SOURCE, source->ID);
	CheckErr(handle->errhp, OCIStmtPrepare(handle->stmthp, handle->errhp, sqltext, strlen((char*)sqltext), OCI_NTV_SYNTAX, OCI_DEFAULT));
	CheckErr(handle->errhp, OCIStmtExecute(handle->svchp, handle->stmthp, handle->errhp, 1, 0, NULL, NULL, OCI_DEFAULT));
	CheckErr(handle->errhp, OCITransCommit(handle->svchp, handle->errhp, 0));
	int rows_processed = 0;
	OCIAttrGet(handle->stmthp, OCI_HTYPE_STMT, &rows_processed, 0, OCI_ATTR_ROW_COUNT, handle->errhp);
	printf("已经成功修改%d行\n",rows_processed);
	return 0;
}
int  Oci_Envir_Create(Ocihandle *handle){	
	text* dbname = (text*) "orcl";	         //数据库服务名
	text* username = (text*) "c##liu";   //用户名	
	text* password = (text*) "123456";      //用户口令
	/*使用线程和对象模式来创建环境句柄	*/
	OCIEnvCreate(&(handle->envhp), OCI_THREADED|OCI_OBJECT, (dvoid *)0,0,0,0, (size_t) 0, (dvoid **)0);	
	/*分配服务器句柄	*/
	OCIHandleAlloc ((dvoid *)(handle->envhp), (dvoid **)&(handle->srvhp),OCI_HTYPE_SERVER, 0, (dvoid **) 0);	
	/*分配错误句柄	*/
	OCIHandleAlloc ((dvoid *)(handle->envhp), (dvoid **)&(handle->errhp),OCI_HTYPE_ERROR, 0, (dvoid **) 0);	
	/*创建服务器上下文句柄*/
	if (OCIServerAttach (handle->srvhp, handle->errhp, (text *)dbname,strlen((char*)dbname), OCI_DEFAULT) != OCI_SUCCESS)
	{
		printf("连接数据库失败!\n"); 
		return -1;
	}
	else
		printf("链接数据库成功！\n");	
	/*分配服务器上下文句柄*/
	OCIHandleAlloc ((dvoid *)(handle->envhp), (dvoid **)&(handle->svchp),OCI_HTYPE_SVCCTX, 0, (dvoid **) 0);	
	/*设置服务器上下文句柄的服务器句柄属性*/
	OCIAttrSet ((dvoid *)(handle->svchp), OCI_HTYPE_SVCCTX,(dvoid *)(handle->srvhp), (ub4) 0, OCI_ATTR_SERVER, handle->errhp);	
	/*分配用户会话句柄	*/
	OCIHandleAlloc ((dvoid *)(handle->envhp), (dvoid **)&(handle->usrhp),OCI_HTYPE_SESSION, 0, (dvoid **) 0);	
	/*为用户会话句柄设置用户名和密码属性*/	
	OCIAttrSet ((dvoid *)(handle->usrhp), OCI_HTYPE_SESSION,(dvoid *)username,(ub4)strlen((char*)username),OCI_ATTR_USERNAME, handle->errhp);	
	OCIAttrSet ((dvoid *)(handle->usrhp), OCI_HTYPE_SESSION,(dvoid *)password,(ub4)strlen((char*)password),OCI_ATTR_PASSWORD, handle->errhp);
	return 0;
}
int connect_server(Ocihandle *handle)
{
    if (OCISessionBegin ( handle->svchp,handle->errhp,handle->usrhp,OCI_CRED_RDBMS, OCI_DEFAULT) != OCI_SUCCESS)
    {
		return -1;            	
    } 
    /*在服务器上下文环境中设置用户会话属性*/
    OCIAttrSet ( (dvoid *)(handle->svchp), OCI_HTYPE_SVCCTX,(dvoid *)(handle->usrhp), (ub4) 0, OCI_ATTR_SESSION,handle->errhp);
    /*分配语句句柄*/
    CheckErr(handle->errhp, OCIHandleAlloc(handle->envhp, (void**)&(handle->stmthp), OCI_HTYPE_STMT, 0, 0));

}
int disconnect_server(Ocihandle *handle)
{
	OCISessionEnd((OCISvcCtx*)handle->svchp, handle->errhp, handle->usrhp,OCI_DEFAULT);    
	OCIServerDetach((OCIServer*)handle->srvhp, handle->errhp, OCI_DEFAULT);  
        printf("断开链接！\n");
	return 1;
}
void cleanup_oci(Ocihandle *handle)
{    if(handle->stmthp)       OCIHandleFree(handle->stmthp, OCI_HTYPE_STMT);
	if(handle->usrhp)         OCIHandleFree(handle->usrhp, OCI_HTYPE_SESSION);
	if(handle->svchp)         OCIHandleFree(handle->svchp, OCI_HTYPE_SVCCTX);
	if(handle->srvhp)          OCIHandleFree(handle->srvhp, OCI_HTYPE_SERVER);
	if(handle->errhp)          OCIHandleFree(handle->errhp, OCI_HTYPE_ERROR);
	if(handle->envhp)         OCIHandleFree(handle->envhp, OCI_HTYPE_ENV);
       printf("清理句柄！！\n");
}

int  Sel_Source_all(Ocihandle *handle)
{	//Superuser superuser;
	Source source;
	char sqltext[1024];
	sprintf(sqltext,"SELECT  PART_MD5 ,URL_SOURCE, SOURCE_TYPE, ID FROM URL_SOURCE");
	OCIDefine* defhp[4];
#define STRSIZE  1024
	text* selectsql = (text*)sqltext;

	/*准备SQL语句*/
	CheckErr(handle->errhp, OCIStmtPrepare(handle->stmthp, handle->errhp, selectsql, strlen((char*)selectsql),OCI_NTV_SYNTAX, OCI_DEFAULT));

	/*输出绑定*/
	CheckErr(handle->errhp, OCIDefineByPos(handle->stmthp, &defhp[0],handle->errhp, 1, &source.PART_MD5, STRSIZE,SQLT_STR, 0, 0, 0, OCI_DEFAULT));
	CheckErr(handle->errhp, OCIDefineByPos(handle->stmthp, &defhp[1],handle->errhp, 2, &source.URL_SOURCE, STRSIZE,SQLT_STR, 0, 0, 0, OCI_DEFAULT));
	CheckErr(handle->errhp, OCIDefineByPos(handle->stmthp, &defhp[2],handle->errhp, 3, &source.SOURCE_TYPE, sizeof(source.SOURCE_TYPE),SQLT_STR, 0, 0, 0, OCI_DEFAULT));
	CheckErr(handle->errhp, OCIDefineByPos(handle->stmthp, &defhp[3],handle->errhp, 4, &source.ID, sizeof(source.ID),SQLT_STR, 0, 0, 0, OCI_DEFAULT));

	CheckErr(handle->errhp,OCIStmtExecute(handle->svchp,handle->stmthp,handle->errhp,1,0,NULL,NULL,OCI_STMT_SCROLLABLE_READONLY));
	printf("\n查询结果为：\n");
	int rows_fetched;
	int i;
	do{
          printf("%s\n %s\n %s\n %s\n",source.ID, source.URL_SOURCE, source.SOURCE_TYPE, source.PART_MD5);
	}
	while( OCI_NO_DATA != OCIStmtFetch2(handle->stmthp, handle->errhp, 1,OCI_FETCH_NEXT,1, OCI_DEFAULT));
#undef STRSIZE
	return 0;
}
int  Build_hash(Ocihandle *handle, PHash_table H)
{
	PFile_info file = (PFile_info)malloc(sizeof(struct File_info));
	char sqltext[1024];
	sprintf(sqltext, "select FILE_NAME,FILE_INFO.PART_MD5,FULL_MD5,URL_SOURCE,SOURCE_TYPE from FILE_INFO,URL_SOURCE where FILE_INFO.PART_MD5=URL_SOURCE.PART_MD5");
	OCIDefine* defhp[5];
#define STRSIZE 1024
	text* selectsql = (text*)sqltext;

	CheckErr(handle->errhp, OCIStmtPrepare(handle->stmthp, handle->errhp, selectsql, strlen((char*)selectsql), OCI_NTV_SYNTAX, OCI_DEFAULT));
	
	CheckErr(handle->errhp, OCIDefineByPos(handle->stmthp, &defhp[0], handle->errhp, 1, file->name, STRSIZE, SQLT_STR, 0, 0, 0, OCI_DEFAULT));
	CheckErr(handle->errhp, OCIDefineByPos(handle->stmthp, &defhp[1], handle->errhp, 2, file->part_md5, STRSIZE, SQLT_STR, 0, 0, 0, OCI_DEFAULT));
	CheckErr(handle->errhp, OCIDefineByPos(handle->stmthp, &defhp[2], handle->errhp, 3, file->md5, STRSIZE, SQLT_STR, 0, 0, 0, OCI_DEFAULT));
	CheckErr(handle->errhp, OCIDefineByPos(handle->stmthp, &defhp[3], handle->errhp, 4, file->url, STRSIZE, SQLT_STR, 0, 0, 0, OCI_DEFAULT));
	CheckErr(handle->errhp, OCIDefineByPos(handle->stmthp, &defhp[4], handle->errhp, 5, &file->type, sizeof(file->type), SQLT_STR, 0, 0, 0, OCI_DEFAULT));
	
	CheckErr(handle->errhp, OCIStmtExecute(handle->svchp, handle->stmthp, handle->errhp, 1, 0, NULL, NULL, OCI_STMT_SCROLLABLE_READONLY));
	printf("================Install file in hash as follow================\n");
	do{
		install(H, file);

		//printf("NAME=======%s\n PART_MD5========%s\n MD5===========%s\n  URL===========%s\n  TYPE=========%d\n", file->name, file->part_md5, file->md5, file->url, file->type);
	}
	while(OCI_NO_DATA != OCIStmtFetch(handle->stmthp, handle->errhp, 1, OCI_FETCH_NEXT, OCI_DEFAULT));
		
#undef STRSIZE
	free(file);
	file = NULL; 
	return 0;
}
int  Sel_Source(Ocihandle *handle, char* md5)
{	//Superuser superuser;
	Source source;
	printf("获得的md5值为:---------->>>>>>>>>%s\n",md5);
	char get_md5[1024] ;
	strcpy(get_md5,md5);
	printf("查询的MD5值为：%s\n",get_md5);
	char sqltext[1024];
	sprintf(sqltext,"SELECT PART_MD5, URL_SOURCE, SOURCE_TYPE, ID  FROM URL_SOURCE WHERE PART_MD5=:m");
	OCIDefine* defhp[4];
	OCIBind* bndhp;
#define STRSIZE  1024
	text* selectsql = (text*)sqltext;
	/*准备SQL语句*/
	CheckErr(handle->errhp, OCIStmtPrepare(handle->stmthp, handle->errhp, selectsql, strlen((char*)selectsql),OCI_NTV_SYNTAX, OCI_DEFAULT));
	/*输入绑定*/
	CheckErr(handle->errhp, OCIBindByName(handle->stmthp, &bndhp, handle->errhp, (text *) ":m", (sb4) strlen((char *) ":m"), (dvoid *) &get_md5, STRSIZE, SQLT_STR, (dvoid *) 0, 0, 0, 0 ,0, OCI_DEFAULT));
	/*输出绑定*/
	CheckErr(handle->errhp, OCIDefineByPos(handle->stmthp, &defhp[0],handle->errhp, 1, &source.PART_MD5, STRSIZE,SQLT_STR, 0, 0, 0, OCI_DEFAULT));
	CheckErr(handle->errhp, OCIDefineByPos(handle->stmthp, &defhp[1],handle->errhp, 2, &source.URL_SOURCE, STRSIZE,SQLT_STR, 0, 0, 0, OCI_DEFAULT));
	CheckErr(handle->errhp, OCIDefineByPos(handle->stmthp, &defhp[2],handle->errhp, 3, &source.SOURCE_TYPE, sizeof(source.SOURCE_TYPE),SQLT_STR, 0, 0, 0, OCI_DEFAULT));
	CheckErr(handle->errhp, OCIDefineByPos(handle->stmthp, &defhp[3],handle->errhp, 4, &source.ID, sizeof(source.ID),SQLT_STR, 0, 0, 0, OCI_DEFAULT));
//	printf("\n查询结果为：\n");
//	printf("ID  URL_SOURCE  SOURCE_TYPE\n");
	CheckErr(handle->errhp, OCIStmtExecute(handle->svchp, handle->stmthp, handle->errhp, 1, 0, NULL, NULL,OCI_STMT_SCROLLABLE_READONLY));
	//printf("%d\n",OCIStmtFetch(handle->stmthp, handle->errhp, 1, OCI_FETCH_NEXT, OCI_DEFAULT));
	do{
		printf("ID---->%s\n URL_SOURCE----->%s\n SOURCE----->%s\n PART_MD5---->%s\n",source.ID, source.URL_SOURCE, source.SOURCE_TYPE, source.PART_MD5);
	}
	while(OCI_NO_DATA != OCIStmtFetch(handle->stmthp, handle->errhp, 1, OCI_FETCH_NEXT, OCI_DEFAULT));
#undef STRSIZE
	return 0;
}

int  Sel_user(Ocihandle *handle, char* md5)
{	//Superuser superuser;
//	Source source;
	User  user;
	strcpy(user.u_name,md5);
	printf("获得的u_name值为:---------->>>>>>>>>%s\n",md5);
	//char get_md5[1024] ;
	//strcpy(get_md5,md5);
	//printf("查询的MD5值为：%s\n",get_md5);
	char sqltext[1024];
	sprintf(sqltext,"SELECT u_psd FROM NEW_USER WHERE u_name =:m");
	OCIDefine* defhp;
	OCIBind* bndhp;
#define STRSIZE  255
	text* selectsql = (text*)sqltext;
	/*准备SQL语句*/
	CheckErr(handle->errhp, OCIStmtPrepare(handle->stmthp, handle->errhp, selectsql, strlen((char*)selectsql),OCI_NTV_SYNTAX, OCI_DEFAULT));
	/*输入绑定*/
	CheckErr(handle->errhp, OCIBindByName(handle->stmthp, &bndhp, handle->errhp, (text *) ":m", (sb4) strlen((char *) ":m"), (dvoid *) user.u_name,STRSIZE, SQLT_STR, (dvoid *) 0, 0, 0, 0 ,0, OCI_DEFAULT));
	/*输出绑定*/
	CheckErr(handle->errhp, OCIDefineByPos(handle->stmthp, &defhp,handle->errhp, 1, user.u_psd, STRSIZE,SQLT_STR, 0, 0, 0, OCI_DEFAULT));
	//CheckErr(handle->errhp, OCIDefineByPos(handle->stmthp, &defhp[1],handle->errhp, 2, &source.URL_SOURCE, STRSIZE,SQLT_STR, 0, 0, 0, OCI_DEFAULT));
	//CheckErr(handle->errhp, OCIDefineByPos(handle->stmthp, &defhp[2],handle->errhp, 3, &source.SOURCE_TYPE, sizeof(source.SOURCE_TYPE),SQLT_STR, 0, 0, 0, OCI_DEFAULT));
	//CheckErr(handle->errhp, OCIDefineByPos(handle->stmthp, &defhp[3],handle->errhp, 4, &source.ID, sizeof(source.ID),SQLT_STR, 0, 0, 0, OCI_DEFAULT));
//	printf("\n查询结果为：\n");
//	printf("ID  URL_SOURCE  SOURCE_TYPE\n");
	CheckErr(handle->errhp, OCIStmtExecute(handle->svchp, handle->stmthp, handle->errhp, 1, 0, NULL, NULL,OCI_STMT_SCROLLABLE_READONLY));
	//printf("%d\n",OCIStmtFetch(handle->stmthp, handle->errhp, 1, OCI_FETCH_NEXT, OCI_DEFAULT));
	do{
		printf("u_psd---->%s\n ",user.u_psd);
	}
	while(OCI_NO_DATA != OCIStmtFetch(handle->stmthp, handle->errhp, 1, OCI_FETCH_NEXT, OCI_DEFAULT));
#undef STRSIZE
	return 0;
}
//int insert_superuser(Ocihandle *handle,int id,char *name,char *pwd)
int  Add_Source(Ocihandle *handle, Source *source) //添加源信息条目
{
	char insertsql[1024];
	sprintf(insertsql,"INSERT INTO URL_SOURCE(PART_MD5, URL_SOURCE, SOURCE_TYPE, ID)VALUES ('%s', '%s', '%s', '%s')",source->PART_MD5, source->URL_SOURCE, source->SOURCE_TYPE, source->ID);
	CheckErr(handle->errhp, OCIStmtPrepare(handle->stmthp, handle->errhp, insertsql, strlen((char*)insertsql),OCI_NTV_SYNTAX, OCI_DEFAULT));
	CheckErr(handle->errhp, OCIStmtExecute(handle->svchp, handle->stmthp, handle->errhp, 1, 0, NULL, NULL,OCI_DEFAULT));

}
