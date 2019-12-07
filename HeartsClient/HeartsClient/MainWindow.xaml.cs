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
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace HeartsClient
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public Socket client;        
        public byte[] bytes = new byte[1024];

        int whoIam = 0;
        bool isMyTurn = false;
        bool quitGame = false;
        bool canIWait = false;

        public MainWindow()
        {
            InitializeComponent();            
        }

        public void ConnectClient()
        {
            IPAddress ipAddress = IPAddress.Parse("192.168.56.101");
            client = new Socket(ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp);           
            client.Connect("192.168.56.101", 12345);
            textBlock1.Text += "[+]Socket kapcsolódott: " + client.RemoteEndPoint.ToString() + Environment.NewLine;            

            //RECEIVE
            int bytesRec = client.Receive(bytes);
            string receivedMessage = Encoding.ASCII.GetString(bytes, 0, bytesRec);

            if (receivedMessage.Equals("HelloP1"))
            {
                whoIam = 1;
                isMyTurn = true;
                textBlock1.Text += " [-]1.játékos vagyok! Várakozás a 2.játékosra!\n";                                
            }
            else if (receivedMessage.Equals("HelloP2"))
            {
                whoIam = 2;

                textBlock1.Text += " [-]2.játékos vagyok!\n";
                textBlock1.Text += "[+]A játék elkezdődött!\n";

                SendMessage("START");                
            }
            canIWait = true;
        }

        void SendMessage(string message)
        {
            byte[] msg = Encoding.UTF8.GetBytes(message);
            int bytesSent = client.Send(msg);
        }

        void ReceiveMessage()
        {
            int bytesRec = client.Receive(bytes);
            string receivedMessage = Encoding.ASCII.GetString(bytes, 0, bytesRec);
            CompareMessage(receivedMessage);
        }
                
        void CompareMessage(string recMessage)
        {
            if (recMessage.Equals("cards1"))
            {
                textBlock1.Text += "  [-]Kártyáim:\n";
            }
            else if(recMessage.Equals("cards2"))
            {
                textBlock1.Text += "  [-]Kártyáim2:\n";
            }
        }

        private async Task DelayedReceiveTask()
        {
            await Task.Delay(100);
            if (!quitGame && canIWait)
            {
                ReceiveMessage();
            }
        }

        private void ConnectButton_Click(object sender, EventArgs e)
        {
            ConnectClient();

            DelayedReceiveTask();
        }

        private void EndGameButton_Click(object sender, EventArgs e)
        {
            SendMessage("VEGE");
            quitGame = true;
            client.Shutdown(SocketShutdown.Both);
            client.Close();
            this.Close();
        }

        private void GiveUpButton_Click(object sender, RoutedEventArgs e)
        {
            SendMessage("FELADOM");
        }

        private void RetryButton_Click(object sender, RoutedEventArgs e)
        {
            SendMessage("UJRA");
        }
    }
}
