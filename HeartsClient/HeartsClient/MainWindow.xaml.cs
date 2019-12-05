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

namespace HeartsClient
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public Socket client;

        // Data buffer for incoming data.  
        public byte[] bytes = new byte[1024];

        public int bytesRec;

        public int whoIamB = 0;
        public int isP2Connected = 0;
        public int game = 0;
        public string receivedMsg = "";

        public MainWindow()
        {
            InitializeComponent();
        }

        public void ConnectClient()
        {
            IPHostEntry ipHostInfo = Dns.GetHostEntry(Dns.GetHostName());            
            IPAddress ipAddress = Dns.GetHostEntry("zrobi-VBuntu").AddressList[1];
            IPEndPoint server = new IPEndPoint(ipAddress, 12345);            
            client = new Socket(ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp);            
            client.Connect(server);
            textBlock1.Text += "[+]Socket connected to " + client.RemoteEndPoint.ToString() + Environment.NewLine;
            
            
            //RECEIVE
            int bytesRec = client.Receive(bytes);
            string receivedMessage = Encoding.ASCII.GetString(bytes, 0, bytesRec);

            if (receivedMessage.Equals("HelloP1"))
            {
                textBlock1.Text += " [-]1.játékos vagyok! Várakozás a 2.játékosra!";
            }
            else if (receivedMessage.Equals("HelloP2"))
            {
                textBlock1.Text += " [-]2.játékos vagyok!";
            }            
        }

        void SendMessage(string message)
        {
            byte[] msg = Encoding.UTF8.GetBytes(message);
            int bytesSent = client.Send(msg);
        }

        private void ConnectButton_Click(object sender, EventArgs e)
        {
            ConnectClient();
        }

        private void EndGameButton_Click(object sender, EventArgs e)
        {
            client.Shutdown(SocketShutdown.Both);
            client.Close();
            this.Close();
        }

        private void GiveUpButton_Click(object sender, RoutedEventArgs e)
        {

        }

        private void RetryButton_Click(object sender, RoutedEventArgs e)
        {

        }
    }
}
