//////////////////////////////////////////////////////////////////////////////////
// ClientAction.xaml.cs -This is the Main Console  which allows clients	        //
//                       to Push and pull packages to the remote repository     //
//                                                                              //
//																				//
// Language:    Visual C#,														//
// Platfrom:    Lenovo U410 Ultrabook Win7x64									//
// Application: CSE687 - Object Oriented Design, Spring 2013                    //
// Author:		Mohit Bhalla Syracuse University					            //
// Instructor:	Jim Fawcett, CST 2-187, Syracuse University                     //
//              (315) 443-3948, jfawcett@twcny.rr.com                           //
//////////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.IO;
using System.Threading;
using System.Windows.Threading;
using System.Windows.Forms;

namespace ClientGUI
{
    /// <summary>
    /// Interaction logic for ClientAction.xaml
    /// </summary>
    /// 

    public partial class ClientAction : Window
    {

        private CommChannel.ICommChannel com1;

        private int ClientPortNo = 5123;//client application listens at this port

        private Thread clientReceiveThread;

        private Action<string> myItemDelegate;

        String[] selectedFileList;
        private FolderBrowserDialog fbd;
        
        public ClientAction(string ipAddress,string portNo,CommChannel.ICommChannel com)
        {
            InitializeComponent();

            System.Windows.MessageBox.Show("Connected to Server a IP:Port#" + ipAddress + ":" + portNo);

            com1 = com;

        
            com1.startRcvr(ClientPortNo);

        }


        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            myItemDelegate =this.ServerSays;

            clientReceiveThread = new Thread(new ThreadStart(this.Run));//C# thread is the receiver!
            clientReceiveThread.IsBackground = true;
            clientReceiveThread.Start();
           
            fbd = new FolderBrowserDialog();
           
        }


        private void ServerSays(string incomingMessage)
        {
             System.Windows.MessageBox.Show("Server Says:- "+incomingMessage);
        }

        private void Run()
        {
            while (true)
            {
                string msg = com1.getMessage();

                if (msg != "")
                    Dispatcher.Invoke(myItemDelegate, DispatcherPriority.Background, new object[] { msg });
            }
        }


        private void FileOpenDialog(object sender, RoutedEventArgs e)
        {
            selectedFileList = new String[100];

            fbd.ShowNewFolderButton = false;

            fbd.SelectedPath = Directory.GetCurrentDirectory();

            DialogResult result = fbd.ShowDialog();

            if (result == System.Windows.Forms.DialogResult.OK)
            {
                Directory.SetCurrentDirectory(fbd.SelectedPath);

                string[] headers = Directory.GetFiles(fbd.SelectedPath, "*.h");
                string[] cpps = Directory.GetFiles(fbd.SelectedPath, "*.cpp");

                SelectedFiles.Items.Clear();
                foreach (string file in headers)
                {
                    if (file != null)
                    {
                        SelectedFiles.Items.Add(System.IO.Path.GetFileName(file));
                    }
                }

                foreach (string file in cpps)
                {
                    if (file != null)
                    {
                        SelectedFiles.Items.Add(System.IO.Path.GetFileName(file));
                     }
                }

            }

        }

        /*
         * The simple idea, select 2 files , give a namespace id to them
         * then for each file, 
         * 
         * Outer Message:-
         *      
         *          PUSH-PKG
         *          PkGNAME
         *          
         *          PUSH-File
         *          File Contents
         *          EOF 
         *          
         *          PUSH-File
         *          File Contents
         *          EOF 
         *          
         *          PUSH-File
         *          File Contents
         *          EOF 
         *          
         *          EOPush
         */
            
        private void PushFiles(object sender, RoutedEventArgs e)
        {
            int ctr = 0;

            foreach (Object item in SelectedFiles.SelectedItems)
            {
                string file = item as string;
                selectedFileList[ctr] = fbd.SelectedPath + "\\" + file;
                ctr++;
            }
            com1.startRcvr(ClientPortNo);
            if (ctr > 0)
            {
                if (NamespaceID.Text == null || NamespaceID.Text == "DefaultNS" || PackageName.Text == null || PackageName.Text == "DefaultPkg")
                {
                    System.Windows.MessageBox.Show("Please write a unique Namespace/package combination");
                }

                else
                {
                    com1.postMessage("PUSH-PKG");

                    com1.postMessage(NamespaceID.Text + "|" + PackageName.Text);

                    try
                    {
                        for (int i = 0; i < ctr; i++)
                        {
                            com1.sendFile(selectedFileList[i]);
                        }
                    }

                    catch (Exception ex)
                    {
                        String exception = ex.ToString();
                    }

                    com1.postMessage("EOPUSH-PKG");
                }
            }

            else
            {
                System.Windows.MessageBox.Show("Please Select Files");
            }

        }

        private void ExtractPackage(object sender, RoutedEventArgs e)
        {
            System.Windows.MessageBox.Show("Unimplemented Method Stub");
        }



        private void Window_KeyDown(object sender, System.Windows.Input.KeyEventArgs e)
        {
            if (e.Key == Key.Return)
            {
                PushFiles(sender, e);
                e.Handled = true;
            }
        }


    }



}
