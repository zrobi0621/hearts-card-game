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
        bool quitGame = false;
        bool canIWait = false;
        bool isStarted = false;

        List<int> myCards = new List<int>();
        Image[] myCardsImages;

        int p1PickedCard = 0;
        int p2PickedCard = 0;
        int p3PickedCard = 0;
        int p4PickedCard = 0;

        int turn = 0;
        string[] tokenTmp;

        public MainWindow()
        {
            InitializeComponent();
            myCardsImages = new Image[]{Card0, Card1, Card2, Card3, Card4, Card5, Card6, Card7, Card8, Card9, Card10, Card11, Card12};
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
                //isMyTurn = true;
                WhoIAmLabel.Content = "1.Játékos";
                TLLabel.Content = "2.Játékos \\/";
                TRLabel.Content = "(= 3.Játékos";
                BRLabel.Content = "4.játékos /\\";

                textBlock1.Text += " [-]1.játékos vagyok! Várakozás a 2.játékosra!\n";                                
            }
            else if (receivedMessage.Equals("HelloP2"))
            {
                whoIam = 2;
                WhoIAmLabel.Content = "2.Játékos";
                TLLabel.Content = "3.Játékos \\/";
                TRLabel.Content = "(= 4.Játékos";
                BRLabel.Content = "1.játékos /\\";

                textBlock1.Text += " [-]2.játékos vagyok!\n";
                textBlock1.Text += "[+]A játék elkezdődött!\n";

                SendMessage("START");                
            }
            canIWait = true;
        }

        void SendMessage(string message)
        {            
            textBlock1.Text += message + "\n";
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
            string[] token = recMessage.Split('.');
            if (token[0].Equals("C1"))
            {                
                isStarted = true;
                turn = 0;
                LoadMyCards(token);
            }
            else if(token[0].Equals("C2"))
            {                
                isStarted = true;
                turn = 0;
                LoadMyCards(token);
            }
            else if(token[0].Equals("T1"))
            {                
                p2PickedCard = int.Parse(token[1]);
                p3PickedCard = int.Parse(token[2]);
                p4PickedCard = int.Parse(token[3]);
                textBlock1.Text += "T1: " + token[1] + " " + token[2] + " " + token[3]+"\n";
                PickedCards();
                tokenTmp = token;
            }
            else if (token[0].Equals("T2"))
            {
                p1PickedCard = int.Parse(token[1]);
                p3PickedCard = int.Parse(token[2]);
                p4PickedCard = int.Parse(token[3]);
                textBlock1.Text += "T2: " + token[1] + " " + token[2] + " " + token[3]+"\n";
                PickedCards();
                tokenTmp = token;
            }
            else if(token[0].Equals("WIN"))
            {
                if (MessageBox.Show(token[1] + ".játékos nyert! Újra?", "Question", MessageBoxButton.YesNo, MessageBoxImage.Warning) == MessageBoxResult.Yes)
                {
                    if (whoIam == 1)
                    {
                        ResetGame();
                    }
                    else if (whoIam == 2)
                    {
                        SendMessage("START");
                    }
                }
                else
                {
                    SendMessage("VEGE");
                    quitGame = true;
                    client.Shutdown(SocketShutdown.Both);
                    client.Close();
                    this.Close();
                }
            }
            else if(token[0].Equals("VEGE"))
            {
                if (whoIam == 1)
                {
                    textBlock1.Text += "A 2.játékos kilépett, a játéknak VÉGE!";
                    MessageBox.Show("A 2.játékos kilépett, a játéknak VÉGE!");
                    this.Close();
                }
                else if(whoIam == 2)
                {
                    textBlock1.Text += "Az 1.játékos kilépett, a játéknak VÉGE!";
                    MessageBox.Show("Az 1.játékos kilépett, a játéknak VÉGE!");
                    this.Close();
                }
            }
            else if (token[0].Equals("FELADOM"))
            {
                if (whoIam == 1)
                {
                    textBlock1.Text += "A 2.játékos feladta!";
                    if (MessageBox.Show("Az 2.játékos feladta! Új játék?", "Question", MessageBoxButton.YesNo, MessageBoxImage.Warning) == MessageBoxResult.Yes)
                    {
                        if (whoIam == 1)
                        {
                            ResetGame();
                        }
                        else if (whoIam == 2)
                        {
                            SendMessage("START");
                        }                        
                    }
                    else
                    {
                        SendMessage("VEGE");
                        quitGame = true;
                        client.Shutdown(SocketShutdown.Both);
                        client.Close();
                        this.Close();
                    }
                }
                else if (whoIam == 2)
                {
                    textBlock1.Text += "Az 1.játékos feladta!";
                    MessageBox.Show("Az 1.játékos feladta!");
                }
            }
            else if (token[0].Equals("UJRA"))
            {
                if (whoIam == 1)
                {
                    textBlock1.Text += "A 2.játékos újrakezedené!";
                    if (MessageBox.Show("A 2.játékos újrakezdené! Új játék?", "Question", MessageBoxButton.YesNo, MessageBoxImage.Warning) == MessageBoxResult.Yes)
                    {                        
                        ResetGame();                        
                    }
                    else
                    {
                        SendMessage("VEGE");
                        quitGame = true;
                        client.Shutdown(SocketShutdown.Both);
                        client.Close();
                        this.Close();
                    }
                }
                else if (whoIam == 2)
                {
                    textBlock1.Text += "Az 1.játékos újrakezdené!";
                    if (MessageBox.Show("Az 1.játékos újrakezdené! Új játék?", "Question", MessageBoxButton.YesNo, MessageBoxImage.Warning) == MessageBoxResult.Yes)
                    {
                        ResetTurn();
                        SendMessage("START");
                        DelayedReceiveTask();
                    }
                    else
                    {
                        SendMessage("VEGE");
                        quitGame = true;
                        client.Shutdown(SocketShutdown.Both);
                        client.Close();
                        this.Close();
                    }
                }
            }
        }

        private void ResetTurn()
        {
            myCards.Clear();
            p1PickedCard = 0;
            p2PickedCard = 0;
            p3PickedCard = 0;
            p4PickedCard = 0;

            CardTop.Source = null;
            CardLeft.Source = null;
            CardRight.Source = null;
            CardBottom.Source = null;

            isStarted = false;

            textBlock1.Text = "";
        }

        private void ResetGame()
        {
            ResetTurn();            
            DelayedReceiveTask();
        }

        private void PickedCards()
        {
            if (whoIam == 1)
            {                
                CardLeft.Source = new BitmapImage(new Uri(@"/Resources/Cards/(" + p2PickedCard.ToString() + ").jpg", UriKind.Relative));
                CardTop.Source = new BitmapImage(new Uri(@"/Resources/Cards/(" + p3PickedCard.ToString() + ").jpg", UriKind.Relative));
                CardRight.Source = new BitmapImage(new Uri(@"/Resources/Cards/(" + p4PickedCard.ToString() + ").jpg", UriKind.Relative));
            }
            else if(whoIam == 2)
            {
                CardLeft.Source = new BitmapImage(new Uri(@"/Resources/Cards/(" + p3PickedCard.ToString() + ").jpg", UriKind.Relative));
                CardTop.Source = new BitmapImage(new Uri(@"/Resources/Cards/(" + p4PickedCard.ToString() + ").jpg", UriKind.Relative));
                CardRight.Source = new BitmapImage(new Uri(@"/Resources/Cards/(" + p1PickedCard.ToString() + ").jpg", UriKind.Relative));
            }
        }

        private void LoadMyCards(string[] token)
        {
            textBlock1.Text += "  [-]Kártyáim: ";

            for (int i = 1; i < 14; i++)
            {
                myCards.Add(int.Parse(token[i]));
                textBlock1.Text += int.Parse(token[i]) + " ";
            }        

            textBlock1.Text += "\n";

            for (int i = 0; i < myCardsImages.Length; i++)
            {
                myCardsImages[i].Source = new BitmapImage(new Uri(@"/Resources/Cards/(" + myCards[i].ToString() + ").jpg", UriKind.Relative));
            }
            
            if (!CanIStart())
            {                
                DelayedReceiveTask();
            }
        }

        private bool CanIStart()
        {
            //TREFF 2
            if (myCards.Contains(41))
            {
                textBlock1.Text += "   [-]Én kezdek, nálam a Treff 2!\n";
                return true;
            }
            else
            {
                textBlock1.Text += "   [-]Nem én kezdek!\n";
                return false;
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
            if (MessageBox.Show("Feladtad! Új játék?", "Question", MessageBoxButton.YesNo, MessageBoxImage.Warning) == MessageBoxResult.Yes)
            {
                SendMessage("UJRA");
            }
            else
            {
                SendMessage("VEGE");
                quitGame = true;
                client.Shutdown(SocketShutdown.Both);
                client.Close();
                this.Close();
            }
        }

        private void RetryButton_Click(object sender, RoutedEventArgs e)
        {
            SendMessage("UJRA");
        }

        private void CardImage_Click(object sender, MouseButtonEventArgs e)
        {
            Image image = sender as Image;
            string name = image.Name;
            name = name.Substring(4);
            int pickedImage = int.Parse(name);

            if (whoIam == 1)
            {
                SendMessage("P1P" + "." + myCards[pickedImage]);
            }
            else if(whoIam == 2)
            {
                SendMessage("P2P" + "." + myCards[pickedImage]);
            }

            image.Source = null;
            CardBottom.Source = new BitmapImage(new Uri(@"/Resources/Cards/(" + myCards[pickedImage] + ").jpg", UriKind.Relative));

            turn++;

            if (turn == 13 && whoIam == 2)
            {
                DelayedReceiveTask();
                
                ResetTurn();
                
                SendMessage("START");                                                                    
            }
            else if(turn == 13 && whoIam == 1)
            {                                
                DelayedReceiveTask();

                ResetTurn();

                DelayedReceiveTask();                
            }
            else
            {
                DelayedReceiveTask();
            }            
        }
    }
}
