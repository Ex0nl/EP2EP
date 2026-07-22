# EP2EP
A lightweight, End-to-End Encrypted (E2EE) Peer-to-Peer chat application that uses libsodium for secure communication and bore for TCP tunneling across NATs.

## Prerequisites for Compilation
(Dependencies to compile and run this project)

- Compiler g++ (or any C++17 compatible compiler)
- Libraries: libsodium-dev (for encryption)
- Tools: curl, tar, and grep (should be preinstalled on any linux system)

### Installation on Debian/Ubuntu
```
sudo apt update
sudo apt install g++ libsodium-dev curl tar grep
```

### Installation on Gentoo
```
sudo emerge --ask dev-libs/libsodium sys-apps/curl app-arch/tar sys-apps/grep
```

## Setup & Compilation
1. Clone the Repository
```
git clone https://github.com/Ex0nl/EP2EP.git
cd EP2EP
```
2. Compile the Program
```
g++ -o ep2ep main.cpp -lsodium -lpthread
```

## Usage
- Run the application in the terminal.
```
./ep2ep
```
- Enter the port for your listener. E.g. 51717 (most commnon)
- The program will automatically download and extract the necessary bore binary on the first run.
- Once the tunnel is established, the program will output the domain and remote port (e.g., bore.pub:12345).
- Share this bore.pub domain and your assigned port with your friend.
- Next you will be prompted to write the peers IP, in most cases, where you use Bore's tunnel, it's "bore.pub".
- After that you will be prompted to write the peers Port, the one they got from Bore.
- Once connected, start typing. All messages are encrypted with a pre-shared key before being sent over the TCP tunnel, you can change the encryption key by changing it in main.cpp and compiling the project.
