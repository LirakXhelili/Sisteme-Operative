# Sisteme-Operative
Projekti ne sisteme operative

# Komunikimi Klient-Server përmes TCP në C

Ky depo përmban dy projekte në gjuhën C që ilustrojnë komunikimin klient-server duke përdorur protokollin TCP/IP. Një projekt është për serverin, ndërsa tjetri për klientin.

## Serveri

Projekti i serverit është shkruar në skedarin server.c. Ky server pranon lidhje nga klientët dhe i përgjigjet me një mesazh të caktuar. Më poshtë janë disa detaje të rëndësishme për këtë projekt:

1. Serveri dëgjon në portin 5002.
2. Mundëson pranimin e deri në 10 klientëve të njëkohshëm.
3. Përdor thread-et për të shqiptuar dhe pranuar mesazhe nga klientët.
4. Shfaq dhe përpuno mesazhet e pranuara nga klientët.
5. Ka një thread shtesë për të dërguar mesazhe nga serveri në klientët.


## Klienti

Projekti i klientit është shkruar në skedarin client.c. Ky klient është i lidhur me serverin në IP adresën "127.0.0.1" dhe portin 5002. Përdoruesi mund të dërgojë kërkesa në server dhe të marrë përgjigje. Më poshtë janë disa detaje të rëndësishme për këtë projekt:

1. Klienti pranon inputin e përdoruesit dhe e dërgon atë në server.
2. Nëse përdoruesi shkruan "disconnect", klienti i dërgon një kërkesë serverit për të mbyllur lidhjen dhe pastaj mbyllet.
3. Klienti merr përgjigjet nga serveri dhe i shfaq ato në konsolë.


## Përdorimi

1. Kompiloni kodin duke përdorur komandën gcc:
   gcc server.c -o server

2. Ekzekutoni serverin:
   ./server

3. Serveri do të dëgjojë në portin 5002 për lidhje nga klientët dhe do t'i përgjigjet me një mesazh të caktuar.

4. Kompiloni kodin duke përdorur komandën gcc:
    gcc client.c -o client

5. Ekzekutoni klientin:
   ./client

6. Klienti do të kërkojë lidhjen me serverin në IP adresën dhe portin e caktuar. 
   Përdoruesi mund të dërgojë kërkesat e tij dhe do të marrë përgjigjet nga serveri.
   
   
## Përfundim

Kodet e serverit dhe klientit në gjuhën C janë zgjidhje të thjeshta dhe funksionale për komunikimin klient-server përmes protokollit TCP/IP. Përdorimi i këtyre kodeve ofron një bazë të mirë për zhvillimin e aplikacioneve që përdorin komunikim në rrjet. Nëse dëshironi të zgjeroni funksionalitetin e aplikacionit ose të adaptuar kodin për nevojat tuaja specifike, mund ta bëni me lehtësi duke përdorur këto kode si pikënisje.


