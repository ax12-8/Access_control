
import pickle
from time import sleep
import time 
from PyQt5.QtWidgets import *
import threading
import sys
from PyQt5.QtCore import *
from PyQt5.QtWidgets import QFileDialog, QMessageBox, QDockWidget, QListWidget
from PyQt5.QtGui import *
import face_recognition
import cv2
import os
from sshtrans import *
import getfacelib
from socket import *
import weather_req
import shutil
from getfacelib import *



req = weather_req.weatherreq()

#初始化socket
address="192.168.43.197"   
port=8266         
buffsize=1024        
s=socket(AF_INET, SOCK_STREAM)

def loadfiles():
    img = getfacelib.getfaceimg()
    img.getallurl()
    img.get_files()
    img.sshconn.sshexit()

def updatedir():
    path = r'C:\Users\Lenovo\Desktop\ProjectOutputs\facerecog\face_dlib_py37_42\images\face'
    shutil.rmtree(path)  
    os.mkdir(path) 
 
def connsock(s):
    #连接socket
    try:
        s.connect((address,port))
        print("连接成功")
       
        return True
        
    except:
        print("连接失败")

        return False
#进行socket连接
    
def recieve_data():
    recvdata = "loaded"
    print(recvdata)
    while recvdata:
        recvdata=s.recv(buffsize).decode('utf-8')
        print("\n  reci："+recvdata)
        if recvdata == "weather":
            send = req.ret_weather()
            s.send(send.encode())
        if recvdata == "time":
            send = req.ret_time()
            s.send(send.encode())
#接收单片机发送过来的信息

# 窗口主类
class MainWindow(QTabWidget):
    # 基本配置不动，然后只动第三个界面
    def __init__(self):
        # 初始化设置
        super().__init__()
        self.setWindowTitle('门禁打卡系统')
        self.resize(1100, 650)
        # 要上传的图片路径
        self.db = getfaceimg()
        self.up_img_name = ""
        self.up_camimg_name = ""
        # 要检测的图片名称
        self.input_fname = ""
        # 要检测的视频名称
        self.source = ''
        self.video_capture = cv2.VideoCapture(0)
        # 初始化中止事件
        self.stopEvent = threading.Event()
        self.stopEvent.clear()
        self.camstopEvent = threading.Event()
        self.camstopEvent.clear()
        # 初始化人脸向量
        self.known_names, self.known_encodings = self.initFaces()
        # 加载lbp检测器
        # 加载人脸识别模型
        # 初始化界面
        self.initUI()
        self.set_down()
        self.db.facedb.connect()
        self.ssh = sshconn()
    # 初始化数据库的人脸
    def initFaces(self):
        # 存储知道人名列表
        known_names = []
        # 存储知道的特征值
        known_encodings = []
        # 遍历存储人脸图片的文件夹
        db_folder = "C:\\Users\\Lenovo\\Desktop\\ProjectOutputs\\facerecog\\face_dlib_py37_42\\images\\face"
        face_imgs = os.listdir(db_folder)
        # 遍历图片，将人脸图片转化为向量
        for face_img in face_imgs:
            face_img_path = os.path.join(db_folder, face_img)
            face_name = face_img.split(".")[0]
            load_image = face_recognition.load_image_file(face_img_path)  # 加载图片
            image_face_encoding = face_recognition.face_encodings(load_image)[0]  # 获得128维特征值
            known_names.append(face_name)  # 添加到人名的列表
            known_encodings.append(image_face_encoding)  # 添加到向量的列表
        return known_names, known_encodings
        
    # 初始化界面
    def initUI(self):
        # 设置字体
        font_v = QFont('楷体', 14)
        generally_font = QFont('楷体', 15)
        # 图片检测
        img_widget = QWidget()
        img_layout = QVBoxLayout()
        img_f_title = QLabel("上传人脸图像")  # 设置标题
        img_f_title.setAlignment(Qt.AlignCenter)  # 设置标题位置为居中
        img_f_title.setFont(QFont('楷体', 18))  # 设置标题字体大小
        # todo 要上传的人脸图像
        self.img_f_img = QLabel()  # 设置第一个界面上要显示的图片
        # self.img_f_img.setPixmap(QPixmap("C:\\Users\\Lenovo\\Desktop\\ProjectOutputs\\facerecog\\face_dlib_py37_42\\UI_images\\renlian1.jpg"))  # 初始化要显示的图片
        self.img_f_img.setAlignment(Qt.AlignCenter)  # 设置图片居中
        self.face_name = QLineEdit()  # 设置当前图片对应的人名
        self.img_opencam_btn = QPushButton("打开摄像头") # 打开cv摄像头，实现实时上传
        self.img_closecam_btn = QPushButton("关闭摄像头") # 关闭摄像头按钮
        img_up_btn = QPushButton("上传图片")  # 设置上传图片的按钮
        img_update_btn = QPushButton("更新人脸库")#从服务器上下载人脸数据
        self.img_det_btn = QPushButton("开始上传")  # 设置开始上传的按钮
        img_up_btn.clicked.connect(self.up_img)  # 联系到相关函数
        self.img_det_btn.clicked.connect(self.up_db_img)  # 连接到相关函数
        # 设置组件的样式
        img_up_btn.setFont(generally_font)
        self.img_det_btn.setFont(generally_font)
        self.img_closecam_btn.setFont(generally_font)
        self.img_opencam_btn.setFont(generally_font)
        img_update_btn.setFont(generally_font)
        img_update_btn.setStyleSheet("QPushButton{color:white}"
                                 "QPushButton:hover{background-color: rgb(2,110,180);}"
                                 "QPushButton{background-color:rgb(48,124,208)}"
                                 "QPushButton{border:2px}"
                                 "QPushButton{border-radius:5px}"
                                 "QPushButton{padding:5px 5px}"
                                 "QPushButton{margin:5px 5px}")
        self.img_closecam_btn.setStyleSheet("QPushButton{color:white}"
                                 "QPushButton:hover{background-color: rgb(2,110,180);}"
                                 "QPushButton{background-color:rgb(48,124,208)}"
                                 "QPushButton{border:2px}"
                                 "QPushButton{border-radius:5px}"
                                 "QPushButton{padding:5px 5px}"
                                 "QPushButton{margin:5px 5px}")
        self.img_opencam_btn.setStyleSheet("QPushButton{color:white}"
                                 "QPushButton:hover{background-color: rgb(2,110,180);}"
                                 "QPushButton{background-color:rgb(48,124,208)}"
                                 "QPushButton{border:2px}"
                                 "QPushButton{border-radius:5px}"
                                 "QPushButton{padding:5px 5px}"
                                 "QPushButton{margin:5px 5px}")
        img_up_btn.setStyleSheet("QPushButton{color:white}"
                                 "QPushButton:hover{background-color: rgb(2,110,180);}"
                                 "QPushButton{background-color:rgb(48,124,208)}"
                                 "QPushButton{border:2px}"
                                 "QPushButton{border-radius:5px}"
                                 "QPushButton{padding:5px 5px}"
                                 "QPushButton{margin:5px 5px}")
        self.img_det_btn.setStyleSheet("QPushButton{color:white}"
                                  "QPushButton:hover{background-color: rgb(2,110,180);}"
                                  "QPushButton{background-color:rgb(48,124,208)}"
                                  "QPushButton{border:2px}"
                                  "QPushButton{border-radius:5px}"
                                  "QPushButton{padding:5px 5px}"
                                  "QPushButton{margin:5px 5px}")
        # 将组件添加到布局上，然后设置主要的widget为当前的布局
        img_layout.addWidget(img_f_title)
        img_layout.addWidget(self.img_f_img)
        img_layout.addWidget(self.face_name)
        img_layout.addWidget(img_up_btn)
        img_layout.addWidget(self.img_det_btn)
        img_layout.addWidget(self.img_opencam_btn)
        img_layout.addWidget(self.img_closecam_btn)
        img_layout.addWidget(img_update_btn)
        img_widget.setLayout(img_layout)
        self.img_closecam_btn.clicked.connect(self.closecam)
        self.img_opencam_btn.clicked.connect(self.open_uploadcam)
        img_update_btn.clicked.connect(self.updateimg)
        '''
        *** 4. 人脸识别界面 ***
        '''
        
        self.post_flag = False
        self.face_names=[]
        self.zippedface = []
        self.imgflag = True
        video_widget = QWidget()
        video_layout = QVBoxLayout()
        # 设置视频识别区的标题
        self.video_title2 = QLabel("人脸识别区")
        self.video_title2.setFont(font_v)
        self.video_title2.setAlignment(Qt.AlignCenter)
        self.video_title2.setFont(font_v)
        # 设置显示的界面
        self.DisplayLabel = QLabel()
        self.DisplayLabel.setPixmap(QPixmap(""))
        self.btn_open_rsmtp = QPushButton("检测摄像头")
        self.btn_open_rsmtp.setFont(font_v)
        # 设置打开摄像头的按钮和样式
        self.btn_open_rsmtp.setStyleSheet("QPushButton{color:white}"
                                          "QPushButton:hover{background-color: rgb(2,110,180);}"
                                          "QPushButton{background-color:rgb(48,124,208)}"
                                          "QPushButton{border:2px}"
                                          "QPushButton{border-radius:5px}"
                                          "QPushButton{padding:5px 5px}"
                                          "QPushButton{margin:5px 5px}")
        # 设置选择文件的的按钮和样式
        self.btn_post = QPushButton("打卡")
        self.btn_post.setFont(font_v)
        self.btn_post.setStyleSheet("QPushButton{color:white}"
                                    "QPushButton:hover{background-color: rgb(2,110,180);}"
                                    "QPushButton{background-color:rgb(48,124,208)}"
                                    "QPushButton{border:2px}"
                                    "QPushButton{border-radius:5px}"
                                    "QPushButton{padding:5px 5px}"
                                    "QPushButton{margin:5px 5px}")
        # 设置结束演示的按钮和样式
        self.btn_close = QPushButton("结束检测")
        self.btn_close.setFont(font_v)
        self.btn_close.setStyleSheet("QPushButton{color:white}"
                                     "QPushButton:hover{background-color: rgb(2,110,180);}"
                                     "QPushButton{background-color:rgb(48,124,208)}"
                                     "QPushButton{border:2px}"
                                     "QPushButton{border-radius:5px}"
                                     "QPushButton{padding:5px 5px}"
                                     "QPushButton{margin:5px 5px}")
        # 将组件添加到布局上
        self.btn_open_rsmtp.clicked.connect(self.open_local)
        self.btn_post.clicked.connect(self.post)
        self.btn_close.clicked.connect(self.close)
        video_layout.setAlignment(Qt.AlignCenter)
        video_layout.addWidget(self.video_title2)
        video_layout.addWidget(self.DisplayLabel)
        self.DisplayLabel.setAlignment(Qt.AlignCenter)
        video_layout.addWidget(self.btn_open_rsmtp)
        video_layout.addWidget(self.btn_post)
        video_layout.addWidget(self.btn_close)
        video_widget.setLayout(video_layout)
        sshconn = sshtrans.sshconn()
        
        # 分别添加子页面
        self.addTab(img_widget, "上传人脸")
        self.addTab(video_widget, '视频检测')
        # self.addTab(about_widget, '关于')
        self.setTabIcon(0, QIcon('UI_images\\图片.png'))
        # self.setTabIcon(1, QIcon('UI_images/图片.png'))
        self.setTabIcon(1, QIcon('UI_images\\直播.png'))
        self.setTabIcon(2, QIcon('UI_images\\logo_about.png'))

    # 第一个界面的函数
    def up_img(self):
   
        # 打开文件选择框
        openfile_name = QFileDialog.getOpenFileName(self, '选择文件', '', 'Image files(*.jpg , *.png)')
        # 获取上传的文件名称
        img_name = openfile_name[0]
        if img_name == '':
            pass
        else:
            # 上传之后显示并做归一化处理
            src_img = cv2.imread(img_name)
            src_img_height = src_img.shape[0]
            src_img_width = src_img.shape[1]
            target_img_height = 400
            ratio = target_img_height / src_img_height
            target_img_width = int(src_img_width * ratio)
            # 将图片统一处理到高为400的图片，方便在界面上显示
            target_img = cv2.resize(src_img, (target_img_width, target_img_height))
            cv2.imwrite("C:\\Users\\Lenovo\\Desktop\\ProjectOutputs\\facerecog\\face_dlib_py37_42\\UI_images\\tmp\\toup.jpg", target_img)
            self.img_f_img.setPixmap(QPixmap("C:\\Users\\Lenovo\\Desktop\\ProjectOutputs\\facerecog\\face_dlib_py37_42\\UI_images\\tmp\\toup.jpg"))
            self.up_img_name = "C:\\Users\\Lenovo\\Desktop\\ProjectOutputs\\facerecog\\face_dlib_py37_42\\UI_images\\tmp\\toup.jpg"

    def up_db_img(self):
        # 首先判断该图像是否有一个人脸，多个人脸或者没有人脸都不行
        face_name = self.face_name.text()
        if face_name == "":
            QMessageBox.information(self, "不能为空", "请填写人脸姓名")
        else:
            if self.img_opencam_btn.isEnabled(): #如果摄像头开启按钮还能用，说明此时采用上传图片的方式，反之则准备用拍摄的图片
                load_image = face_recognition.load_image_file(self.up_img_name)  # 加载图片
            else:
                load_image = face_recognition.load_image_file(self.up_camimg_name)
            image_face_encoding = face_recognition.face_encodings(load_image)  # 获得128维特征值
           
            encoding_length = len(image_face_encoding)  # 获取人脸得数量
            if encoding_length == 0:  # 如果没有人脸，提示用户重新上传
                QMessageBox.information(self, "请重新上传", "当前图片没有发现人脸")
            elif encoding_length > 1:  # 如果人脸有多个，也提示用户重新上传
                QMessageBox.information(self, "请重新上传", "当前图片发现多张人脸")
            else:
                face_encoding = image_face_encoding[0]  # 获取解析得到得人脸数量
            if self.img_opencam_btn.isEnabled(): #如果摄像头开启按钮还能用，说明此时采用上传图片的方式，反之则准备用拍摄的图片
                img = cv2.imread(self.up_img_name)  # 将上传得图片保存在db目录下
                url = self.up_img_name
            else:
                img = cv2.imread(self.up_camimg_name)  # 将上传得图片保存在db目录下
                url = self.up_camimg_name
            ret = "/face/" + face_name + ".jpg"
            img_path = face_name + '.jpg'
            self.ssh.addfilename(ret)
            self.ssh.putfiles(url)
            cv2.imwrite("C:\\Users\\Lenovo\\Desktop\\ProjectOutputs\\facerecog\\face_dlib_py37_42\\images\\face\\" + img_path, img)
            # 上传之后重新对字典进行处理
            
            self.known_names.append(face_name)
            self.known_encodings.append(face_encoding)


            sql = "insert into userinfo (name,url) values('" + face_name + "','" + ret + "')"
            print(sql)
            while True:
                try:
                    self.db.facedb.cursor.execute(sql)
                    self.db.facedb.db.commit()
                    self.imgflag = True
                    break;
                except Exception as error:
                    self.db.facedb.cursor.ping(True)
            self.db.facedb.cursor.execute(sql)
            self.db.facedb.db.commit()
            QMessageBox.information(self, "上传成功", "数据已上传！")
#人脸识别相关
    # 关闭事件 询问用户是否退出
    def closeEvent(self, event):
        reply = QMessageBox.question(self,
                                     '退出',
                                     "是否要退出程序？",
                                     QMessageBox.Yes | QMessageBox.No,
                                     QMessageBox.No)
        if reply == QMessageBox.Yes:
            updatedir()
            self.close()
            event.accept()
        else:
            event.ignore()

    # 打卡函数
    def post(self):
        
        self.imgflag = False
        sleep(2)
        localimg = "C:\\Users\\Lenovo\\Desktop\\ProjectOutputs\\facerecog\\face_dlib_py37_42\\images\\postlib\\" + str(time.time()).replace(" ","").replace(".","_") + '.jpg'
        shutil.copy('C:\\Users\\Lenovo\\Desktop\\ProjectOutputs\\facerecog\\face_dlib_py37_42\\images\\tmp.jpg',localimg)
        time1 = time.ctime()
        try:
            name = self.zippedface[0][1]  
        except:
            sleep(0.05)
            name = self.zippedface[0][1] 
        sql = "insert into PostHistory (name,time) values('" + name + "','" + time1 + "')"
        print(sql)
        while True:
            try:
                self.db.facedb.cursor.execute(sql)
                self.db.facedb.db.commit()
                self.imgflag = True
                break;
            except Exception as error:
                self.db.facedb.cursor.ping(True)
        if name != "unknown":
            QMessageBox.information(self,"打卡", "打卡成功，您的姓名是"+name+"\n打卡时间"+time1)
            try:
                send = "U"
                s.send(send.encode())
                time.sleep(10)
                send = "D"
                s.send(send.encode())
                print("send successful!")
            except:
                print("send failed!")
                
    #上传界面的label获取摄像头数据，并且打开对应线程
    def open_uploadcam(self):
        self.video_capture = cv2.VideoCapture(0,cv2.CAP_DSHOW)
        print(self.video_capture.isOpened())
        th = threading.Thread(target=self.upload_faceimg)
        th.start()

    def open_local(self):
        ## 选择录像文件进行读取
        mp4_filename = 0
        self.source = mp4_filename
        # 读取摄像头进行实时得显示
        self.video_capture = cv2.VideoCapture(self.source)
        th = threading.Thread(target=self.display_video)
        th.start()
    #更新人脸库函数
    def updateimg(self):
        th = threading.Thread(target=self.updateimgth)
        th.start()
        th.join()
        QMessageBox.information(self, "更新成功", "数据已更新！")
    def updateimgth(self):
        updatedir()
        loadfiles()
        
    # 退出进程
    def close(self):
        # 点击关闭按钮后重新初始化界面
        self.stopEvent.set()
        self.set_down()
    def closecam(self):
        self.camstopEvent.set()
        self.set_downcam()


    #打开摄像头上传人脸进程
    def upload_faceimg(self):
        self.img_opencam_btn.setEnabled(False)
        self.img_closecam_btn.setEnabled(True)
        process_this_frame = True
        while True:
            ret, frame = self.video_capture.read()  # 读取摄像头
            # opencv的图像是BGR格式的，而我们需要是的RGB格式的，因此需要进行一个转换。
            rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)  # 将图像转化为rgb颜色通道
            process_this_frame = not process_this_frame
            # 保存图片并进行实时的显示
            frame = frame
            frame_height = frame.shape[0]
            frame_width = frame.shape[1]
            frame_scale = 500 / frame_height
            frame_resize = cv2.resize(frame, (int(frame_width * frame_scale), int(frame_height * frame_scale)))
            cv2.imwrite("C:\\Users\\Lenovo\\Desktop\\ProjectOutputs\\facerecog\\face_dlib_py37_42\\images\\camtmp.jpg", frame_resize)
            self.img_f_img.setPixmap(QPixmap("C:\\Users\\Lenovo\\Desktop\\ProjectOutputs\\facerecog\\face_dlib_py37_42\\images\\camtmp.jpg"))
            self.up_camimg_name = "C:\\Users\\Lenovo\\Desktop\\ProjectOutputs\\facerecog\\face_dlib_py37_42\\images\\camtmp.jpg"
            if cv2.waitKey(25) & self.camstopEvent.is_set() == True:
                self.camstopEvent.clear()
                self.img_f_img.clear()
                self.img_closecam_btn.setEnabled(False)
                self.img_opencam_btn.setEnabled(True)
                self.set_downcam()
                break
        self.img_opencam_btn.setEnabled(True)
        self.img_closecam_btn.setEnabled(False)
        self.set_downcam()

    #执行人脸识别主进程
    def display_video(self):
        # 首先把打开按钮关闭
        self.btn_open_rsmtp.setEnabled(False)
        self.btn_close.setEnabled(True)
        process_this_frame = True
        self.zippedface = []
        while self.imgflag:
            ret, frame = self.video_capture.read()  # 读取摄像头
            # opencv的图像是BGR格式的，而我们需要是的RGB格式的，因此需要进行一个转换。
            rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)  # 将图像转化为rgb颜色通道
     
            if process_this_frame:
                face_locations = face_recognition.face_locations(rgb_frame)  # 获得所有人脸位置
                face_encodings = face_recognition.face_encodings(rgb_frame, face_locations)  # 获得人脸特征值
                face_names = []  # 存储出现在画面中人脸的名字
                for face_encoding in face_encodings:  # 和数据库人脸进行对比
                    # 如果当前人脸和数据库的人脸的相似度超过0.5，则认为人脸匹配
                    matches = face_recognition.compare_faces(self.known_encodings, face_encoding, tolerance=0.37)
                    if True in matches:
                        first_match_index = matches.index(True)
                        # 返回相似度最高的作为当前人脸的名称
                        name = self.known_names[first_match_index]
                    else:
                        name = "unknown"
                    face_names.append(name)
            process_this_frame = not process_this_frame
            self.zippedface = list(zip(face_locations, face_names))
            for (top, right, bottom, left), name in zip(face_locations, face_names):
                cv2.rectangle(frame, (left, top), (right, bottom), (0, 0, 255), 2)  # 画人脸矩形框
                # 加上人名标签
                cv2.rectangle(frame, (left, bottom - 35), (right, bottom), (0, 0, 255), cv2.FILLED)
                font = cv2.FONT_HERSHEY_DUPLEX
                cv2.putText(frame, name, (left + 6, bottom - 6), font, 1.0, (255, 255, 255), 1)
            # 保存图片并进行实时的显示
            frame = frame
            frame_height = frame.shape[0]
            frame_width = frame.shape[1]
            frame_scale = 500 / frame_height
            frame_resize = cv2.resize(frame, (int(frame_width * frame_scale), int(frame_height * frame_scale)))
            cv2.imwrite("C:\\Users\\Lenovo\\Desktop\\ProjectOutputs\\facerecog\\face_dlib_py37_42\\images\\tmp.jpg", frame_resize)
            self.DisplayLabel.setPixmap(QPixmap("C:\\Users\\Lenovo\\Desktop\\ProjectOutputs\\facerecog\\face_dlib_py37_42\\images\\tmp.jpg"))
            if cv2.waitKey(25) & self.stopEvent.is_set() == True:
                self.stopEvent.clear()
                self.DisplayLabel.clear()
                self.btn_close.setEnabled(False)
                self.btn_open_rsmtp.setEnabled(True)
                self.set_down()
                break
        self.btn_open_rsmtp.setEnabled(True)
        self.btn_close.setEnabled(False)
        self.set_down()

    # 初始化视频检测界面
    def set_down(self):
        self.video_capture.release()
        cv2.destroyAllWindows()
        self.DisplayLabel.setPixmap(QPixmap("C:\\Users\\Lenovo\\Desktop\\ProjectOutputs\\facerecog\\face_dlib_py37_42\\UI_images\\ae862.jpg"))
    # 初始化拍摄上传界面
    def set_downcam(self):
        self.video_capture.release()
        cv2.destroyAllWindows()
        self.img_f_img.setPixmap(QPixmap("C:\\Users\\Lenovo\\Desktop\\ProjectOutputs\\facerecog\\face_dlib_py37_42\\UI_images\\ae862.jpg"))
if __name__ == "__main__":
    updatedir()
    #将原有的人脸文件夹删除，防止以前的缓存的文件出现
    loadfiles()
    flag = connsock(s)
    print(flag)
    #从服务器上下载已经保存的人脸数据
    if flag:
        sock_t = threading.Thread(target=recieve_data)  
        sock_t.start()
    # 加载页面
    app = QApplication(sys.argv)
    mainWindow = MainWindow()
    mainWindow.show()
    sys.exit(app.exec_())
