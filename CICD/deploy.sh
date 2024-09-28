#!/usr/bin/expect -f

set timeout 20
set ip "192.168.64.21"
set username "evgeny"
set passwd "student21"

spawn scp src/cat/s21_cat src/grep/s21_grep $username@$ip:/home/$username/


expect {
  "*yes/no*" { send -- "yes\r" }
  "*password*" { send -- "$passwd\r" }
}

spawn ssh $username@$ip "echo $passwd | sudo -S mv ~/s21_cat ~/s21_grep /usr/local/bin/"
expect {
  "*password*" { send -- "$passwd\r" }
}
expect eof
