# -*- coding: GB2312 -*-
import sshtrans 
import pymysql
#����getfacedb
#���ã��������ݿ�
class getfacedb(object):
    def __init__(self):
        pass

    
    def connect(self):
        self.db = pymysql.connect(
        host='47.113.146.163', # ��Ҫ���ӵ�ip
        port=3306, # �˿ڣ��ɲ��Ĭ��3306
        user="root", # �û���
        password="123456", # ����
        database='id_rem' # ���ӵ����ݿ���
        )
        print("connected!")
        self.cursor = self.db.cursor() # ����cursor��ִ��SQL���
    def insert_face(self,name):
        ret = "/face/" + name + ".jpg"
        self.cursor.exec("insert into userinfo (name,url) values(?,?)",(name,ret))
        
    def execsql(self,sql):
        self.cursor.execute(sql)
        self.db.commit()

    def retexec(self,sql):
        self.cursor.execute(sql)
        self.db.commit()
        return self.cursor.fetchall()

#�����ã���ȡ�����б�
class getfaceimg(object):
    def __init__(self):

        self.facedb = getfacedb()
        self.facedb.connect()
        self.face = []
        self.url = []
        self.sshconn = sshtrans.sshconn()
        
    def getfacelist(self):
        sql = "select name from userinfo"
        ret = self.facedb.retexec(sql)
        facelist = []
        for item in range(0,len(ret)):
            facelist.append(ret[item][0])
            self.file
        return facelist

    def getallurl(self):
        sql = "select url from userinfo"
        ret = self.facedb.retexec(sql)
        urllist = []
        for item in range(0,len(ret)):
            urllist.append(ret[item][0])
        self.sshconn.filename.extend(urllist)
        print(self.sshconn.filename)
        return urllist

    def get_files(self):
        print(self.sshconn.filename)
        self.sshconn.getfiles()

#��ȡ�ӿ��÷�����

#img = getfaceimg()
#img.getallurl()
#img.get_files()
#sql = "insert into userinfo values('2','xsa','sadasd')"
#img.facedb.execsql(sql)
#ret = img.getfacelist()
#print(img.getfacelist())
#print("\n" + str(len(ret)))