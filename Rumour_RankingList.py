import os
import platform
import re
import time
from datetime import datetime

from lxml import etree
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.keys import Keys

list_rumour=[]   #谣言列表
list_like=[] #点赞数列表

main_page_url = 'http://piyao.sina.cn/'
headers = {'User-Agent':'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) '
                        'Chrome/51.0.2704.63 Safari/537.36'}
cookies = None
request_session = None
if platform.system()=='Windows':
    chrome_driver_path = "chromedriver.exe"
elif platform.system()=='Linux' or platform.system()=='Darwin':
    chrome_driver_path = "./chromedriver"
else:
    print('Unknown System Type. quit...')

chrome_options = Options()

chrome_options.add_argument('--disable-gpu')
driver = webdriver.Chrome(chrome_options=chrome_options, executable_path= chrome_driver_path)

def findtime():
    TheDate = driver.find_elements_by_xpath('//div[@class="day_date"]')
    date=TheDate[-1].text
    mylist=date.split("-")
    day=int(mylist[0]+mylist[1]+mylist[2])
    return day

driver.get(main_page_url)
time.sleep(1)

date=findtime()

driver.maximize_window()
while date>20190620:     #可以更改日期，此处设置为2019年6月20日
    driver.execute_script('window.scrollTo(0, document.body.scrollHeight)')
    time.sleep(1)
    date=findtime()

likes=driver.find_elements_by_xpath('//div[@class="like_text"]')
titles=driver.find_elements_by_xpath('//div[@class="left_title"]')

for title in titles:                #构造谣言列表
    list_rumour.append(title.text)  

for like in likes:                  #构造点赞数列表
    list_like.append(like.text)

ranking_list=dict(map(lambda x,y:[x,y],list_rumour,list_like))  #用谣言与对应的点赞数构造字典
ranking_list_order=sorted(ranking_list.items(),key=lambda x:int(x[1]),reverse=True)#根据点赞数进行排序

for i in range(10):            #将点赞数前10名打印出来
    print(ranking_list_order[i],'\n')

quit()