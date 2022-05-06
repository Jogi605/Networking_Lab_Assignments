For smtp mail:
        gcc -o s smtpmail.c -pthread
        ./s 4000
For popserver:
        gcc -o p popserver.c -pthread
        ./p 5000
       
For mailclient:
      gcc mailclient.c -o c
      ./c 4000
        
