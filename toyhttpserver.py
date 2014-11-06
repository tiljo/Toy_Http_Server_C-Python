from socket import *
import os
os.chdir('/home/tiljo/www')
fd = socket(AF_INET,SOCK_STREAM)
fd.bind(('127.0.0.1',4000))
fd.listen(3)
while True:
	new_fd,addr = fd.accept()
	print 'got connection from ',addr
	if os.fork()==0 :
		fd.close()
		s= new_fd.recvfrom(1000)
		g = s[0]
		print g
		token = g.split(' ')
		path = token[1][1:]
		if token[1][:]=='/':
			path='a.txt'
		print path
		if os.path.isdir(path):
			path = path+'a.txt'
		d = open(path)
		r = d.read()
		size =  os.path.getsize(path)
	
		token = path.split('.')
		print token[1]

		if token[1]=='png':
			mime = 'image/png'
		elif token[1]=='txt':
			mime = 'text/html'
		elif token[1]=='jpeg':
			mime = 'image/jpg'


		head='HTTP/1.1 200 OK\nContent-length: %s\nContent-Type: %s\r\n\n'% (str(size),mime,)
		print head
		new_fd.sendto(head+r,addr)
		new_fd.close()
		exit(0)
	#else:
	new_fd.close()
		
#fd.close()	

