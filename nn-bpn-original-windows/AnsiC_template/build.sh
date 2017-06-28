#build
sudo gcc main.c -lm -fstack-protector-all -Wstack-protector -fno-omit-frame-pointer -o nn-bpn -ggdb

#run
echo "   "
echo "GDB commands:"
echo "(gdb) run"
echo "(gdb) quit"
echo "   "
echo "   "
gdb nn-bpn
