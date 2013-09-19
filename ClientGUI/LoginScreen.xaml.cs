//////////////////////////////////////////////////////////////////////////////////
// LoginScreen.xaml.cs -This is the login screen which allows clients	        //
//                       to connect to the remote server repository             //
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
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Threading;
using System.Windows.Threading;

namespace ClientGUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private CommChannel.ICommChannel com1;

        private int ClientPortNo = 5123;//client application listens at this port
        
        private Boolean alreadyConnected = false;
        private Thread clientReceiveThread;

        private Action<string> myItemDelegate;

        private void AuthenticationResponse(string incomingMessage)
        {
            string msg =incomingMessage;
           
            if(msg=="YES")
            {
                this.Hide();

                clientReceiveThread.Abort();

                ClientAction c1 = new ClientAction(IPAddress.Text,PortNo.Text,com1);
                
                this.Close();
           
                c1.Show();

            }

            else if(msg=="NO")
            {
                MessageBox.Show("Server Says: Incorrect ID/Password ,try Again");
            }
        }

        private void Run()
        {
            while (true)
            {
               string msg = com1.getMessage();
                
                if(msg!="")
                    Dispatcher.Invoke(myItemDelegate, DispatcherPriority.Background, new object[] { msg });
            }
        }


        public MainWindow()
        {
            InitializeComponent();
            com1 = CommChannel.ICommChannel.CreateChannel();
        }


        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            myItemDelegate = this.AuthenticationResponse;
            clientReceiveThread = new Thread(new ThreadStart(this.Run));//C# thread is the receiver!
            clientReceiveThread.IsBackground = true;
            clientReceiveThread.Start();
      
            MessageBox.Show("Remote Code Repository \n for Demonstration Purpose\n"
                         + "User Name:Password \n mohit:bhalla  \n jim:fawcett"
                         + "\n elie:elaaraj  \n mehmet:kaya  \n vicky:singh   ");
      
      
        }


        private void  postAuthenticationMsg(String name,String Password)
        {
            
            String line1="AUTH";
            String line2 = com1.getIPAddress() + ":" + ClientPortNo;
            String line3 = name + ":" + Password;

            com1.postMessage(line1);
            com1.postMessage(line2);
            com1.postMessage(line3);
            com1.postMessage("EOA");
        }

        private void LoginButton(object sender, RoutedEventArgs e)
        {
            
            String name = UserNameData.Text;
            String password = PasswordData.Password;


            String currdestip = IPAddress.Text;
            String currdestport = PortNo.Text;

            /*
             Sending the data to the communication channel
             */

            try
            {
                if (alreadyConnected == false)
                {
                    com1.setDestAddress(IPAddress.Text, PortNo.Text);
             
                    com1.startSndr();
                    com1.startRcvr(ClientPortNo);
             
                    postAuthenticationMsg(name, password);

                    alreadyConnected = true;
                }

                else
                {
                    postAuthenticationMsg(name, password);
                    com1.startRcvr(ClientPortNo);
                }

            }

            catch (Exception ex)
            {
               MessageBox.Show(ex.ToString());
            }
            IPAddress.IsReadOnly = true;
            IPAddress.Background = Brushes.Gray;
            PortNo.IsReadOnly = true;
            PortNo.Background = Brushes.Gray;
        }

        private void Window_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
            {
                LoginButton(sender, e);
                e.Handled = true;
            }
        }

    }


}
