#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


#include <windows.h>
#include "tlhelp32.h"
#include <string.h>


//判断进程是否存在
BOOL IsExistProcess(const char*  szProcessName)
{
    PROCESSENTRY32 processEntry32;
    HANDLE toolHelp32Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (((int)toolHelp32Snapshot) != -1)
    {
        processEntry32.dwSize = sizeof(processEntry32);
        if (Process32First(toolHelp32Snapshot, &processEntry32))
        {
            do
            {
                int iLen = 2 * wcslen(processEntry32.szExeFile);
                char* chRtn = new char[iLen + 1];
                //转换成功返回为非负值
                wcstombs(chRtn, processEntry32.szExeFile, iLen + 1);
                if (strcmp(szProcessName, chRtn) == 0)
                {
                    return TRUE;
                }
            } while (Process32Next(toolHelp32Snapshot, &processEntry32));
        }
        CloseHandle(toolHelp32Snapshot);
    }
    //
    return FALSE;
}


BOOL qstring2Char(QString inString, char *outChar)
{
    QByteArray ba = inString.toLatin1(); // must
    char *tempChar=ba.data();
    memcpy(outChar,tempChar,strlen(tempChar) );
    return true;
}


//通过进程名来杀死进程
/*
    return true: process was killed successfully
    return false: process was NOT killed
*/
bool KillProcess(QString procNameString)
{

    char proName[100]={'\0'};
    qstring2Char(procNameString,proName);


    PROCESSENTRY32 pe;
    DWORD id=0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    pe.dwSize = sizeof(PROCESSENTRY32);
    if(!Process32First(hSnapshot,&pe))
    {
        return false;
    }

    //---------------char to wchar_t
       WCHAR p_processName[128] = {0};
       mbstowcs(p_processName,proName,strlen(proName));
       //---------------

    while(1)
    {
        pe.dwSize=sizeof(PROCESSENTRY32);
        if(Process32Next(hSnapshot,&pe)==FALSE)
        {
            break;
        }

        //find processName
        if(wcsicmp(pe.szExeFile, p_processName)==0)
        {
            id = pe.th32ProcessID;
            break;
        }
    }
    CloseHandle(hSnapshot);


    //Kill The Process
    HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, id );
    if (hProcess != NULL)
    {
        TerminateProcess(hProcess,0);
        CloseHandle(hProcess);
        return true;
    }

    return false;
}


/*
    return true: process exist
    return false: process NOT exist
*/
bool MainWindow::findProcess(QString procNameString)
{

    char processNameChar[100]={'\0'};
    qstring2Char(procNameString,processNameChar);

    //qDebug()<<" len:"<<strlen(processNameChar);

    if(IsExistProcess(processNameChar))
    {
        //qDebug()<<" exist!";
        return true;

    }
    else
    {
        //qDebug()<<" Not exist!";
        return false;
    }


}



void MainWindow::on_pushButton_test111_clicked()
{
    QString proc("waveAnalysisClient.exe");

    if(findProcess(proc))
    {
        if(KillProcess(proc))
        {
            qDebug()<<proc<< "--->killed success! ";
        }
        else
        {
            qDebug()<<proc<< "--->killed failed! ";
        }
    }
    else
    {
        qDebug()<<proc<< "--->peocess not exist! ";
    }




}
