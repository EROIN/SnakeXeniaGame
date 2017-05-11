#include <windows.h>
#include <iostream.h>
#include <fstream.h>
#include "Game_private.h"

void Raand(int *,int *);
void Display();                 //display background and refresh
void find(int *,int *);                  //find position
void snake(int,int,int,int);
void Timer(WPARAM wparam);
void eatcheck();
void boundary();
void convert(int p,char c[3]);
void OnPause();
void OnSpeed(int,int);
void direction(int ,int *,int *,int);
void Save();
void open();
void levelscore(int,int);
void BigFood(int,int);

LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

char szClassName[] = "WindowsApp";             //Make the class name into a global variable
char fn[MAX_PATH];
float r=60;int dur=33300;
int x,y;int xbg,ybg;       //for the feed...
int xs=450,ys=250,xf=600,yf=250;
int idir=1,fdir[100]; 
short int turn=0,lvlsc=35;
int regx[100],regy[100];
short int p=0,hs=0,times=0,speed=100,count=0;
char c[3]={48,48,48},h[3]={48,48,48};
BOOL pause=FALSE,todo=FALSE,a=FALSE,sound=TRUE,score=FALSE,alrdysvd=FALSE,bf=FALSE,level=FALSE,ft=TRUE,over=false;

     HDC hdc;
     HBRUSH hbr;
     HPEN hpen;
     PAINTSTRUCT ps; 
     HWND hwnd;
     HMENU hmenu;
     HBITMAP hbmp;
     HGDIOBJ hold;
     BITMAP bmp;
     OPENFILENAME ofn;
     
int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)
{             
    MSG messages;            
    WNDCLASSEX wincl;       
    
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      
    wincl.style = CS_HREDRAW|CS_VREDRAW;                 
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (hThisInstance, MAKEINTRESOURCE(IDI_GAME));
    wincl.hIconSm = LoadIcon (hThisInstance, MAKEINTRESOURCE(IDI_GAME));
    wincl.hCursor = LoadCursor (NULL, IDC_HAND);
    wincl.lpszMenuName = szClassName;                 
    wincl.cbClsExtra = 0;                      
    wincl.cbWndExtra = 0;                          
    wincl.hbrBackground = CreateSolidBrush(RGB(200,200,200));

    if (!RegisterClassEx (&wincl))
    return 0;
    
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "EROIN'S SNAKEXENZIA",       /* Title Text */
           WS_CAPTION|WS_SYSMENU, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           CW_USEDEFAULT,                 /* The programs width */
           CW_USEDEFAULT-10,                 /* and height in pixels */
           NULL,        
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 
           );
    
    MessageBox(0,"Eroin's copyright,\nAll rights reserved!","Caution!",MB_ICONEXCLAMATION);
    
    hmenu=LoadMenu(hThisInstance,szClassName);
    SetMenu(hwnd,hmenu);
    
    ShowWindow (hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);

    HACCEL haccel=LoadAccelerators(hThisInstance,szClassName);
    
    ofn.lStructSize = sizeof (OPENFILENAME) ;
    ofn.hwndOwner = hwnd ;
    ofn.lpstrFilter = ".Snx" ;
    ofn.lpstrFile = fn ;
    ofn.nMaxFile = MAX_PATH ;
    ofn.nMaxFileTitle = MAX_PATH ;
    ofn.lpstrDefExt = ".Snx\0" ;
    
    while (GetMessage (&messages, NULL, 0, 0))
    {
        if(!TranslateAccelerator(hwnd,haccel,&messages))
        {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
        }
    }
    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wparam, LPARAM lParam)
{
    switch (message)                  
    {
        case WM_CREATE:
             {
                 fstream *fsh=new fstream;
                 fsh->open("High Score.bak",ios::in|ios::binary);
                 fsh->read((char *)& hs,sizeof(hs));
                 fsh->close();
                 delete fsh;
                 break;
             }
             
        case WM_DESTROY:
                 KillTimer(hwnd,1);
                 EndPaint(hwnd,&ps);
                 DestroyMenu(hmenu);
                 MessageBox(0,"Un Prodotto di EROIN!","Thanks for playing!",MB_ICONERROR);
                 PostQuitMessage (0);       
                 break;
        
        case WM_PAINT:
                 hdc=BeginPaint(hwnd,&ps);
                 find(&x,&y);
                 SetTimer(hwnd,1,speed,NULL);
                 break;
                 
        case WM_TIMER:
                 Timer(wparam);
                 break;
         
        case WM_KILLFOCUS:
             KillTimer(hwnd,1);
             break;
             
        case WM_SETFOCUS:
             if(!pause&&!over)
             SetTimer(hwnd,1,speed,NULL);
             break;
                     
        case WM_COMMAND:
             switch(LOWORD(wparam))
             {
             case IDM_GAMEPAUSE:
             OnPause();
             break;
             
             case IDM_GAMEPLAY:
             OnPause();
             break;
             
             case IDM_SOUNDON:
             sound=TRUE;
             CheckMenuItem(hmenu,IDM_SOUNDON,MF_CHECKED);
             CheckMenuItem(hmenu,IDM_SOUNDOFF,MF_UNCHECKED);
             break;
             
             case IDM_SOUNDOFF:
             sound=FALSE;
             CheckMenuItem(hmenu,IDM_SOUNDON,MF_UNCHECKED);
             CheckMenuItem(hmenu,IDM_SOUNDOFF,MF_CHECKED);
             break;
             
             case IDM_GAMESPEEDFAST:
             OnSpeed(50,IDM_GAMESPEEDFAST);
             break;
             
             case IDM_GAMESPEEDMEDIUM:
             OnSpeed(100,IDM_GAMESPEEDMEDIUM);
             break;
             
             case IDM_GAMESPEEDSLOW:
             OnSpeed(150,IDM_GAMESPEEDSLOW);
             break;
             
             case ID_GAMESPEED:
             speed==100?SendMessage(hwnd,WM_COMMAND,IDM_GAMESPEEDFAST,0):(speed==50?
             SendMessage(hwnd,WM_COMMAND,IDM_GAMESPEEDSLOW,0):SendMessage(hwnd,WM_COMMAND,IDM_GAMESPEEDMEDIUM,0));
             break;
             
             case IDM_SHOWHS:
             score=TRUE;
             EnableMenuItem(hmenu,IDM_SHOWHS,MF_GRAYED);
             EnableMenuItem(hmenu,IDM_HIDEHS,MF_ENABLED);
             break;
             
             case IDM_HIDEHS:
             score=FALSE;
             EnableMenuItem(hmenu,IDM_HIDEHS,MF_GRAYED);
             EnableMenuItem(hmenu,IDM_SHOWHS,MF_ENABLED);
             break;
             
             case IDM_HIGH:
             score?SendMessage(hwnd,WM_COMMAND,IDM_HIDEHS,0):SendMessage(hwnd,WM_COMMAND,IDM_SHOWHS,0);
             break;
             
             case IDM_FILENEW:
             KillTimer(hwnd,1);
             if(!alrdysvd&&IDYES==MessageBox(0,"Save current game?,","Save Changes?",MB_YESNO|MB_ICONQUESTION|MB_ICONEXCLAMATION))
             SendMessage(hwnd,WM_COMMAND,IDM_FILESAVEAS,0);
             xs=450,ys=250,yf=250;xf=600;
             c[0]=c[1]=c[2]=48;p=0;
             pause=FALSE;todo=FALSE;a=FALSE;level=FALSE;ft=TRUE;alrdysvd=FALSE;
             times=0;idir=1;turn=0;
             EnableMenuItem(hmenu,IDM_GAMEPLAY,MF_GRAYED);
             EnableMenuItem(hmenu,IDM_GAMEPAUSE,MF_ENABLED);
             EndPaint(hwnd,&ps);
             InvalidateRect(hwnd,&ps.rcPaint,TRUE);
             break;
             
             case IDM_FILESAVE:
             if(!alrdysvd)
             SendMessage(hwnd,WM_COMMAND,IDM_FILESAVEAS,0);
             else
             Save();
             break;
             
             case IDM_FILESAVEAS:
             KillTimer(hwnd,1);
             ofn.Flags=OFN_OVERWRITEPROMPT;
             if(GetSaveFileName(&ofn))
             Save();
             if(!pause)
             SetTimer(hwnd,1,speed,NULL);
             break;
             
             case IDM_FILEOPEN:
             KillTimer(hwnd,1);
             ofn.Flags=OFN_READONLY;
             if(GetOpenFileName(&ofn))
             open();
             if(!pause)
             SetTimer(hwnd,1,speed,NULL);
             break;
             
             case IDM_FILEEXIT:
             DestroyWindow(hwnd);
             break;
             
             case IDM_HELPABOUT:
             MessageBox(0,
             "Eroin's Copyright!\n(c)All Rights Reserved!\nGame developed using API functions.It will work only on computers with Microsoft Windows 98+ and will give best performance on Windows XP sp2+","About Eroin's Program",MB_ICONINFORMATION);
             break;
             
             case IDM_HELPHOW:
             MessageBox(0,
             "Use the arrow keys or 'w','s','a','d' for navigation.Press Space to pause or resume.Try to eat the red food as soon as possible before it disappeares to gain extra points depending upon its size.You can also set the speed of the snake and other properties using the menu\nIMPORTANT:\nAlways save the saved games files in a folder in the same directory...","Help!",MB_ICONINFORMATION);
             break;
             
             case IDM_HELPNO:
             WinHelp(hwnd,"HELPFILE.HLP",HELP_CONTENTS,0L);
             break;
             
             case IDM_BFLONG:
             BigFood(51000,IDM_BFLONG);
             break;
             
             case IDM_BFMEDIUM:
             BigFood(33300,IDM_BFMEDIUM);
             break;
             
             case IDM_BFSHORT:
             BigFood(22500,IDM_BFSHORT);
             break;
             
             case IDM_SCORE30:
             levelscore(35,IDM_SCORE30);
             break;
             
             case IDM_SCORE20:
             levelscore(20,IDM_SCORE20);
             break;
             
             case IDM_SCORE40:
             levelscore(60,IDM_SCORE40);
             break;
             }
             
        case WM_KEYDOWN:
             switch(wparam)
             {
                           case 'S':
                           case VK_DOWN:
                           if(turn<100&&fdir[turn-1]!=4&&todo!=TRUE&&(!pause)&&(turn||(idir!=4))) 
                           {  
                           regx[turn]=xf;
                           regy[turn]=yf;
                           fdir[turn]=2;
                           turn+=1;
                           todo=TRUE;
                           }
                           break;
                           
                           case 'A':
                           case VK_LEFT:
                           if(turn<100&&fdir[turn-1]!=1&&todo!=TRUE&&(!pause)&&(turn||(idir!=1))) 
                           {  
                           regx[turn]=xf;
                           regy[turn]=yf;
                           fdir[turn]=3;
                           turn++;
                           todo=TRUE;
                           }
                           break;
                           
                           case 'W':
                           case VK_UP:
                           if(turn<100&&fdir[turn-1]!=2&&todo!=TRUE&&(!pause)&&(turn||(idir!=2))) 
                           {  
                           regx[turn]=xf;
                           regy[turn]=yf;
                           fdir[turn]=4;
                           turn++;
                           todo=TRUE;
                           }
                           break;
                           
                           case 'D':
                           case VK_RIGHT:
                           if(turn<100&&fdir[turn-1]!=3&&todo!=TRUE&&(!pause)&&(turn||(idir!=3))) 
                           {  
                           regx[turn]=xf;
                           regy[turn]=yf;
                           fdir[turn]=1;
                           turn++;
                           todo=TRUE;
                           }
                           break;
                           
                           case VK_SPACE:
                           if(!over)
                           OnPause();
                           break;    
             }   
                 
        default:                      
            return DefWindowProc (hwnd, message, wparam, lParam);
    }

    return 0;
}
void snake(int xs,int ys,int xf,int yf)
{
     hpen=CreatePen(PS_SOLID,10,RGB(0,0,0));
     hold=SelectObject(hdc,hpen);
     MoveToEx(hdc,xs,ys,NULL);
     LineTo(hdc,xf,yf);
     SelectObject(hdc,hold);
     DeleteObject(hpen);
}
void Raand(int *x,int *y)
{
     srand(time(NULL));
     *x=(rand()*rand())%1000;
     *y=(rand()*rand())%480;
}
void Display()
{
     HDC hmemdc;
     /*hbr=CreateSolidBrush(RGB(200,200,2));
     hold=SelectObject(hdc,hbr);
     GetClientRect(hwnd,&ps.rcPaint);
     Rectangle(hdc,ps.rcPaint.left,ps.rcPaint.top,ps.rcPaint.right,ps.rcPaint.bottom);              //0,0,1355,700   //0,0,999,485
     SelectObject(hdc,hold);
     DeleteObject(hbr);*/
     
     hbmp=LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_BKG));
     GetObject(hbmp,sizeof(BITMAP),&bmp);
     hmemdc=CreateCompatibleDC(hdc);
     hold=SelectObject(hmemdc,hbmp);
     GetClientRect(hwnd,&ps.rcPaint);
     //Rectangle(hdc,ps.rcPaint.left,ps.rcPaint.top,ps.rcPaint.right,ps.rcPaint.bottom);              //0,0,1355,700   //0,0,999,485
     StretchBlt(hdc,ps.rcPaint.left,ps.rcPaint.top,ps.rcPaint.right,ps.rcPaint.bottom,hmemdc,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);
     SelectObject(hdc,hold);
     DeleteObject(hbmp);
     DeleteDC(hmemdc);
     
     TextOut(hdc,875,15,"Score",5);
     convert(p,c);
     TextOut(hdc,950,15,c,3);
     
     fstream fsh;
     if(p>hs)
     {    
     if(alrdysvd)
     fsh.open("../High Score.bak",ios::out|ios::binary);
     else
     fsh.open("High Score.bak",ios::out|ios::binary);
     fsh.write((char *)& p,sizeof(p));
     fsh.close();
     hs=p;
     if(times<300)
     SetTimer(hwnd,3,1,NULL);
     }
     if(score)
     {
     convert(hs,h);
     TextOut(hdc,875,40,"High Score",10);
     TextOut(hdc,950,40,h,3);
     }
     if(bf)
     {
     hbr=CreateSolidBrush(RGB(255,0,0));
     hold=SelectObject(hdc,hbr);
     Ellipse(hdc,xbg-r/2,ybg-r/2,xbg+r/2,ybg+r/2);
     SelectObject(hdc,hold);
     DeleteObject(hbr);
     r-=0.45;
     if(r<=0)
     {
     r=60;
     count=0;
     bf=FALSE;
     }
     }
     if(level)
     {
     hbmp=LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_LEVEL));
     hbr=CreatePatternBrush(hbmp);
     hold=SelectObject(hdc,hbr);
     Rectangle(hdc,155,95,325,395);
     Rectangle(hdc,655,95,825,395);
     SelectObject(hdc,hold);
     DeleteObject(hbr);
     DeleteObject(hbmp);
     }
}
void find(int *x,int *y)
{
     Raand(x,y);
     while((*x<ps.rcPaint.left||*y<ps.rcPaint.top||*x>ps.rcPaint.right||*y>ps.rcPaint.bottom-10)||
     (level&&((*x>=150&&*x<=325)||(*x>=650&&*x<=825)&&(*y>=90&&*y<=390))))
     {
     SetCursor(LoadCursor(NULL,IDC_WAIT));      //set wait cursor
     Raand(x,y);
     }
     SetCursor(LoadCursor(NULL,IDC_HAND));
}
void convert(int temp,char c[3])              //int to string
{
     int i=2;
     while(temp!=0)
     {
     c[i]=(temp%10)+48;
     i--;
     temp/=10;
     }
}
void OnPause()
{
     pause=!pause;
     if(pause)
     {
     KillTimer(hwnd,1);
     EnableMenuItem(hmenu,IDM_GAMEPAUSE,MF_GRAYED);
     EnableMenuItem(hmenu,IDM_GAMEPLAY,MF_ENABLED);
     }
     else
     {
     EnableMenuItem(hmenu,IDM_GAMEPLAY,MF_GRAYED);
     EnableMenuItem(hmenu,IDM_GAMEPAUSE,MF_ENABLED);
     SetTimer(hwnd,1,speed,NULL);
     }
}    
void OnSpeed(int s,int id)
{
     speed=s;
     KillTimer(hwnd,1);
     count/=3;
     for(int i=IDM_GAMESPEEDFAST;i<=IDM_GAMESPEEDSLOW;i++)
     i==id?CheckMenuItem(hmenu,i,MF_CHECKED):CheckMenuItem(hmenu,i,MF_UNCHECKED);
     if(!pause)
     SetTimer(hwnd,1,speed,NULL);
}  
void BigFood(int x,int id)
{
     count/=3;
     dur=x;
     for(int i=IDM_BFLONG;i<=IDM_BFSHORT;i++)
     i==id?CheckMenuItem(hmenu,i,MF_CHECKED):CheckMenuItem(hmenu,i,MF_UNCHECKED);
}   
void levelscore(int x,int id)
{
     lvlsc=x;
     for(int i=IDM_SCORE20;i<=IDM_SCORE40;i++)
     i==id?CheckMenuItem(hmenu,i,MF_CHECKED):CheckMenuItem(hmenu,i,MF_UNCHECKED);
}
void direction(int dr,int *xs,int *ys,int l)
{
     if(dr==1)
     *xs+=l;
     else if(dr==2)
     *ys+=l;
     else if(dr==3)
     *xs-=l;
     else if(dr==4)
     *ys-=l;
}
void eatcheck()
{
     BOOL flag=FALSE;
     if(((xf>x&&xf<x+15)&&(yf>y&&yf<y+15)))
     {
     flag=TRUE;
     p++;
     find(&x,&y);
     }
     else if(bf&&((xf>xbg-r/2&&xf<xbg+r/2)&&(yf>ybg-r/2&&yf<ybg+r/2)))
     {
     flag=TRUE;
     p+=r/10;
     bf=FALSE;
     r=60;
     count=0;
     }
     if(flag)
     {
     if(sound)
     PlaySound(MAKEINTRESOURCE(ID_SOUND),GetModuleHandle(NULL),SND_ASYNC|SND_RESOURCE);
     direction(idir,&xs,&ys,-40);
     }
} 
void boundary()
{
     BOOL flag=FALSE;
     if((xf==0||yf==ps.rcPaint.top||xf==1010||yf==480||yf==ps.rcPaint.bottom)||
     (level&&(((xf>=155&&xf<=325)||(xf>=655&&xf<=825))&&(yf>=95&&yf<=395))))
     flag=TRUE;
     for(int i=0;i<turn-2;i++)
     {
     if(((xf>=regx[i]&&xf<=regx[i+1])&&(yf==regy[i]))||((xf>=xs&&xf<=regx[0])&&yf==ys))       //moving right
     flag=TRUE;
     else if(((xf<=regx[i]&&xf>=regx[i+1])&&(yf==regy[i]))||((xf<=xs&&xf>=regx[0])&&yf==ys))
     flag=TRUE;
     else if(((yf>=regy[i]&&yf<=regy[i+1])&&(xf==regx[i]))||((yf>=ys&&yf<=regy[0]&&xf==xs)))
     flag=TRUE;
     else if(((yf<=regy[i]&&yf>=regy[i+1])&&(xf==regx[i]))||((yf<=ys&&yf>=regy[0]&&xf==xs)))
     flag=TRUE;
     }
     if(flag)
     {
     if(sound)
     PlaySound(MAKEINTRESOURCE(ID_CHORD),GetModuleHandle(NULL),SND_RESOURCE|SND_ASYNC);
     over=true;
     KillTimer(hwnd,1);
     MessageBox(0,"Thanks for Playing!","GAME OVER!",MB_OK);
     int id=MessageBox(0,"[ABORT]\tExit\n[RETRY]\tContinue with the saved game\n\t(Starts a new game if none loaded)\n[IGNORE]\tStart new game","Play Again?",MB_ABORTRETRYIGNORE|MB_ICONQUESTION|MB_DEFBUTTON2);
     switch (id)
     {
            case IDIGNORE:
            alrdysvd=TRUE;
            SendMessage(hwnd,WM_COMMAND,IDM_FILENEW,0);
            break;
            
            case IDRETRY:
            if(alrdysvd)
            {
            open();
            over=false;
            SetTimer(hwnd,1,speed,NULL);
            }
            else
            {
            alrdysvd=TRUE;
            SendMessage(hwnd,WM_COMMAND,IDM_FILENEW,0);
            }
            break;
            
            default:
            DestroyWindow(hwnd);     
            }
     }
}
void Timer(WPARAM wparam)
{
     HDC hmemdc;
     switch(wparam)
     {
            case 1:
            count++;
            if((dur/speed)==count)
            {
            bf=TRUE;
            find(&xbg,&ybg);
            }
            Display();
            todo=FALSE;
            a=!a;
            if(p>=lvlsc&&ft)
            {
            KillTimer(hwnd,1);
            level=TRUE;
            ft=FALSE;
            find(&x,&y);
            idir=1;xs=150,xf=300;ys=50;yf=50;turn=0;
            SetTimer(hwnd,1,speed,NULL);
            }
            if(a)
            {
            hbmp=LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_FOOD));
            GetObject(hbmp,sizeof(BITMAP),&bmp);
            hmemdc=CreateCompatibleDC(hdc);
            hold=SelectObject(hmemdc,hbmp);
            StretchBlt(hdc,x,y,15,15,hmemdc,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);
            SelectObject(hdc,hold);
            DeleteObject(hbmp);
            DeleteDC(hmemdc);
            }
            
            if(turn)
            {
            direction(idir,&xs,&ys,10);
            snake(xs,ys,regx[0],regy[0]);
            
            int draw;
            for(draw=0;draw<turn-1;draw++)
            snake(regx[draw],regy[draw],regx[draw+1],regy[draw+1]);
            
            direction(fdir[turn-1],&xf,&yf,10);
            snake(regx[turn-1],regy[turn-1],xf,yf);
            
            if(xs==regx[0]&&ys==regy[0])
            {
                        idir=fdir[0];
                        for(draw=0;draw<turn-1;draw++)
                        {
                        fdir[draw]=fdir[draw+1];
                        regx[draw]=regx[draw+1];
                        regy[draw]=regy[draw+1];
                        }
                        turn-=1;
            }
            }
            
            else
            {
            direction(idir,&xs,&ys,10);
            direction(idir,&xf,&yf,10);
            snake(xs,ys,xf,yf);
            }
            
            hbmp=LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_HEAD));
            GetObject(hbmp,sizeof(BITMAP),&bmp);
            hmemdc=CreateCompatibleDC(hdc);
            hold=SelectObject(hmemdc,hbmp);
            StretchBlt(hdc,xf-4,yf-4,8,8,hmemdc,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);
            SelectObject(hdc,hold);
            DeleteObject(hbmp);
            DeleteDC(hmemdc);
            
            eatcheck();
            boundary();
            break;
            
            case 3:
            if(times<300&&p)
            {
            times++;
            TextOut(hdc,250,450,"HIGHEST SCORE!",14);
            }
            else
            KillTimer(hwnd,3);
            break;
            }
}
void Save()
{
             TextOut(hdc,0,15,fn,strlen(fn));
             alrdysvd=TRUE;
             fstream *fs=new fstream;
             fs->open(fn,ios::out|ios::binary);
             fs->write((char *)& p,sizeof(p)); 
             fs->write((char *)& x,sizeof(x)); 
             fs->write((char *)& y,sizeof(y)); 
             fs->write((char *)& lvlsc,sizeof(lvlsc)); 
             fs->write((char *)& level,sizeof(level)); 
             fs->write((char *)& idir,sizeof(idir));  
             fs->write((char *)& xs,sizeof(xs));  
             fs->write((char *)& ys,sizeof(ys));
             fs->write((char *)& xf,sizeof(xf));
             fs->write((char *)& yf,sizeof(yf));
             fs->write((char *)& turn,sizeof(turn));
             for(int i=1;i<=turn;i++)
             {
             fs->write((char *)& regx[i-1],sizeof(regx[i-1]));
             fs->write((char *)& regy[i-1],sizeof(regy[i-1]));
             fs->write((char *)& fdir[i-1],sizeof(fdir[i-1]));
             }
             fs->close();
             delete fs;
}
void open()
{
             TextOut(hdc,0,15,fn,strlen(fn));
             c[0]=c[1]=c[2]=48;
             fstream *fs=new fstream;
             fs->open(fn,ios::in|ios::binary);
             fs->read((char *)& p,sizeof(p)); 
             fs->read((char *)& x,sizeof(x)); 
             fs->read((char *)& y,sizeof(y)); 
             fs->read((char *)& lvlsc,sizeof(lvlsc));
             levelscore(lvlsc,lvlsc==20?IDM_SCORE20:(lvlsc==35?IDM_SCORE30:IDM_SCORE40));
             fs->read((char *)& level,sizeof(level)); 
             level?ft=FALSE:ft=TRUE;
             fs->read((char *)& idir,sizeof(idir));  
             fs->read((char *)& xs,sizeof(xs));  
             fs->read((char *)& ys,sizeof(ys));
             fs->read((char *)& xf,sizeof(xf));
             fs->read((char *)& yf,sizeof(yf));
             fs->read((char *)& turn,sizeof(turn));
             for(int i=1;i<=turn;i++)
             {
             fs->read((char *)& regx[i-1],sizeof(regx[i-1]));
             fs->read((char *)& regy[i-1],sizeof(regy[i-1]));
             fs->read((char *)& fdir[i-1],sizeof(fdir[i-1]));
             }
             fs->close();
             delete fs;
             times=0;
             alrdysvd=TRUE;
}
