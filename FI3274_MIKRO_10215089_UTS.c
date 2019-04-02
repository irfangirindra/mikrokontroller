#include <reg52.h>

//Inisiasi Variabel dan pin
unsigned int button,menu,submenu,i,j,step, pilihan, nilai;
sbit RS = P1^0;					// RS ada di port P1.0
sbit RW = P1^1;					// RW ada di port P1.1
sbit E = P1^7;					// E ada di port P1.7
//Set interupt button dan pin
sbit up = P1^2;			 		// up ada di port P1.2
sbit down = P1^3;				// down ada di port P1.3
sbit enter = P1^4;			// enter ada di port P1.4
sbit cancel = P1^5;			// cancel ada di port P1.5
sbit get = P1^6;				// get ada di port P1.6
//Initialize default value
unsigned char vstep[4]="7.89";
unsigned char vmax[4]="0.12";
unsigned char sl[1]="4";
unsigned char save[8]=" Save";
unsigned char nsave[9]=" Not Save";
unsigned char channel[1]="5";
unsigned char kirim[10];
//Initialize function
void display(unsigned char *c, unsigned char r);
void delay (unsigned int t);
void port_initial ();
void main_menu();
void lcd_write (unsigned char w);
void lcd_cmd(unsigned int c);
void lcd_initial();
void add(unsigned char *c,unsigned int i);
void sub(unsigned char *c,unsigned int i);
void init_imf();
//Button Interupt
void tombol(void)interrupt 0 {
	//Jika up ditekan
	if (up == 0){
		if(menu==2 & submenu==3){
			submenu=1;
		 	main_menu();
		 	lcd_cmd(0xc0);
		 	pilihan=2;
		}else if(menu==3){
		 	if(submenu==2 && nilai==1){
				lcd_cmd(0x80);
				pilihan=1;;
			}else{
				if(submenu==1 && nilai==1) add(&vstep,pilihan);
				if(submenu==1 && nilai==2) add(&vmax,pilihan);
				if(submenu==1 && nilai==3) add(&sl,pilihan);
				if(submenu==2 && nilai==2) add(&channel,pilihan);
				if(submenu==3) add(&sl, pilihan);
				main_menu();
			}
		}else{
			pilihan=1;
			lcd_cmd(0x80);
		}while(up==0);	
	 }//Jika down ditekan	
	if (down == 0){
		if(menu==2 && submenu == 1 && pilihan == 2){
		 	submenu=3;
		 	main_menu();
		}else if(menu==3){
		 	if(submenu==2 && nilai==1){
				lcd_cmd(0xc0);
				pilihan=2;
			}else{
				if(submenu==1 && nilai==1) sub(&vstep,pilihan);
				if(submenu==1 && nilai==2) sub(&vmax,pilihan);
				if(submenu==1 && nilai==3) sub(&sl,pilihan);
				if(submenu==2 && nilai==2) sub(&channel,pilihan);
				if(submenu==3) sub(&sl, pilihan);
				main_menu();
			}
		}else if(menu==2 & submenu==3){
			submenu=1;
			main_menu();
		}else{
			pilihan=2;
			lcd_cmd(0xc0);
		}while(down==0);
	}//Jika enter ditekan
	if(enter==0){
		if(menu==3){
			if(submenu!=3 && submenu!=2){
				if(pilihan==1) pilihan=3;
				else if(pilihan==3) pilihan=4;
			}else{}
			main_menu();
		}else{
			menu=menu+1;
			if(menu==2) submenu=pilihan;
			if(menu==3) {
				nilai=pilihan;
				pilihan=1;
			}
		}main_menu();
		while(enter==0);
	}//Jika enter ditekan
	if(cancel==0){
	 	if(menu!=1){
			menu=menu-1;
			pilihan = 1;
			main_menu();
			while(cancel==0);
		}
	}//Jika get ditekan
	if(get==0){
	 if(menu==3){
	  	 if(submenu==1){
		  	if(nilai==1){
			  for(i=0;i<4;i++) kirim[i]=vstep[i];
			}else if(nilai==2){
			  for(i=0;i<4;i++) kirim[i]=vmax[i];
			}else if(nilai==3){
			  for(i=0;i<1;i++) kirim[i]=sl[i];
			}
		 }if(submenu==2){
		 	if(nilai==1){
				if(pilihan==1) for(i=0;i<8;i++) kirim[i]=save[i];
				if(pilihan==2) for(i=0;i<9;i++) kirim[i]=nsave[i];
			}else if(nilai==2){
			  for(i=0;i<1;i++) kirim[i]=channel[i];
			}
		}
		 if(RI){
		   RI=0;
		   SBUF=kirim;
		 }
	 }
	 while(get==0);
	}
}
//Main Program
void main(){			
	//Inisialisasi port dan lcd
	port_initial();
	lcd_initial();
	init_imf();
	//Jalankan main menu
	main_menu();
	//Inisialisasi interrupt
	IT0 = 0;
	IE = 0x81;
	//Looping
	while(1);
}
//Menuliskan huruf ke dalam lcd
void lcd_write (unsigned char w){
  RS = 1;             // set RS = 1
  P2 = w;             // Data yang ditransfer
  E  = 1;             // set E high, untuk memberi sinyal high (beri perintah lcd)
  delay(150);
  E  = 0;             // set E low, untuk menutup sinyal (tidak ada perintah lcd)
  delay(150);
}
//Inisiasi kondisi awal, seluruh port dalam keadaan low
void port_initial (){
	P0=0x00;
	P1=0x00;
	P2=0x00;
	step = 1;
	menu = 1;
	submenu = 1;
}
//Delay data
void delay (unsigned int t){
	for (i=0; i<t; i++);
}
//Display menu ke lcd
void display(unsigned char *c, unsigned char r){
	unsigned char i;
	for(i=0;i<r;i++){
		lcd_write(c[i]);
	}
}
//Memberi perintah LCD
void lcd_cmd(unsigned int c){
  RS = 0;             // set RS = 0
  P2 = c;             // Data untuk perintah lcd
  E  = 1;             // set E high, untuk memberi sinyal high (beri perintah lcd)
  delay(150);
  E  = 0;             // set E low, untuk menutup sinyal (tidak ada perintah lcd)
  delay(150);
}
//Inisiasi LCD
void lcd_initial(){
  lcd_cmd(0x38);    //function set
  lcd_cmd(0x0f);    //display on,cursor off,blink off
  lcd_cmd(0x01);    //clear display
  lcd_cmd(0x06);    //entry mode, set increment
	menu = 1;
	submenu = 1;
	pilihan = 1;
	nilai=1;
	up=1;
	down=1;
	enter=1;
	cancel=1;
	get=1;
}
void main_menu(){
	lcd_cmd(0x01);
	if(menu==1){
	   display("Setting 1",10);
	   lcd_cmd(0xc0);
	   display(" File 2",7);
	   lcd_cmd(0x80);
	   pilihan=1;
	}else if(menu==2 && submenu==2){
		display(" File Mode 2.1",14);
		lcd_cmd(0xC0);
		display(" Channel 2.2",12);
		lcd_cmd(0x80);
		pilihan=1;
	}else if(menu==2 && submenu==1){
		display(" Vstep 1.1",10);
		lcd_cmd(0xC0);
		display(" Vmax 1.2",9);
		lcd_cmd(0x80);
		pilihan=1;
	}else if(menu==2 && submenu==3){
		display(" Speed level 1.3",16);
		lcd_cmd(0x80);
		pilihan=1;
	}else if(menu==3 && submenu == 1 && nilai == 1){
	    display("Vstep 1.1",9);
		lcd_cmd(0xC0);
		display(&vstep,4);
	}else if(menu==3 && submenu == 1 && nilai == 2){
	    display("Vmax 1.2",8);
		lcd_cmd(0xC0);
		display(&vmax,4);
	}else if(menu==3 && submenu == 3){
	    display("Speed level 1.3",15);
		lcd_cmd(0xC0);
		display(&sl,1);
	}else if(menu==3 && submenu == 2 && nilai == 1){
	    display(&save,5);
	   	lcd_cmd(0xC0);
		display(&nsave,9);
	}else if(menu==3 && submenu == 2 && nilai == 2){
	   display("Channel 2.2",11);
	   lcd_cmd(0xC0);
	   display(&channel,1);
	}if(menu==3){
	 	if(pilihan==1) {
			if(menu==3 && submenu == 2 && nilai == 1) lcd_cmd(0x80);
			else lcd_cmd(0xc0);
		}if(pilihan==3) lcd_cmd(0xc2);
		if(pilihan==4) lcd_cmd(0xc3);
	}	
}
//Add value
void add(unsigned char *c,unsigned int i){
 unsigned char p;
 p=c[i-1]+1;
 c[i-1]=p;
}
//Substract value
void sub(unsigned char *c,unsigned int i){
 unsigned char p;
 p=c[i-1]-1;
 c[i-1]=p;
}
//Initialize UART
void init_imf(){
	SCON =0x50;
	RCAP2H=0xff;
	RCAP2L=0xee; //nilai-nilai reload timer
	T2CON=0x34;  //baudrate generator berjalan
}

