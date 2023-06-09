from fileinput import filename
import paramiko
 


class sshconn(object):
        def __init__(self, server = "47.113.146.163", username="root", password="P@ssw0rd", timeout=1):
            self.server = server  # ssh远程连接的服务器ip
            self.filename = []
            self.ssh = paramiko.SSHClient()
            self.ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
            self.ssh.connect(self.server, username="root", password="P@ssw0rd")
            self.sftp = self.ssh.open_sftp()
            print("初始化成功！")

        def addfilename(self,newlist):
            self.filename.extend(newlist)

        def addfilename(self,filename):
            self.filename.append(filename)

        def putfiles(self,localpath):
            remotepath = r''
            for i in self.filename:
               localpath1 = localpath 
                #服务器的文件路径
               remotepath1 = remotepath + i 
               print(self.server,'开始数据传输')
               self.sftp.put(localpath1, remotepath1, callback = None)
               print('数据传输完成')
                # self.ssh.close()
            self.filename = []

        def getfiles(self):
            remotepath = r''
            localpath = r'C:\\Users\\Lenovo\\Desktop\\ProjectOutputs\\facerecog\\face_dlib_py37_42\\images\\'
            for i in self.filename:
                localpath1 = localpath + i
                #服务器的文件路径
                remotepath1 = remotepath + i
                #print(localpath1)
                #print(remotepath1)
                print(self.server,'开始数据传输')
                self.sftp.get(remotepath1,localpath1)
                print('数据传输完成')
            self.filename = []
        
        def sshexit(self):
            self.ssh.close()
            print("退出成功！")
for item in range(0,50):
    ssh = sshconn()
    ssh.sshexit()
#files = ["WJQ.jpg","SSP.jpg"]
#ssh.addfilename(files)
#remot = r'/face/'
#locel = r'C:\\Users\\Lenovo\\Desktop\\ProjectOutputs\\facerecog\\face_dlib_py37_42\\images\\faces\\'
#ssh.getfiles(locel,remot)