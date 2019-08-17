import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Random;

public class Network {
    private DatagramSocket socket;
    private InetAddress group;
    private byte[] buf;
    private ArrayList<String[]> servers;
    
    protected Socket socktcp;


    protected String theAddress;
    protected int thePort;
	private PanGame panGame;
	private ReceiveTCP recTCP;
	private Frame frame;
	private boolean willingQuit;
	private boolean isIA;

    
    public Network(Frame f){
    	frame = f;
    	willingQuit = true;
	}
    
	public class ClosingtcpThread extends Thread {

		public void run() {
			try {
				socktcp.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
	
	public int openSocketTCP(String addr,int port){
		theAddress = addr;
		thePort = port;
		try {
			socktcp = new Socket(InetAddress.getByName(addr),port);
		} catch (IOException e) {
			return -1;
		}
		return 1;
	}
	
	public void listeningTCP(){
		recTCP= new ReceiveTCP();
		recTCP.start();
	}
	
	public int sendTCP(String address,int port,String msg){
		try {
			BufferedWriter socketWriter=null;
			socketWriter = new BufferedWriter(new OutputStreamWriter(socktcp.getOutputStream()));
			Thread.sleep(50);
			socketWriter.write(msg);
			socketWriter.flush();
			return 1;

		} catch (IOException | InterruptedException e2) {
			return -1;
		}
	}
	
	public void parse(String s){
		/*String s= "POST /game/infos-player/2 HTTP/1.1\nhost: "+theAddress+":"+thePort+"\nuser-agent: seat 1\ncontent-Length:20\n\n"+
				"[name=Nicocas:position=6:money=660.5:"
				+"isPlaying=1:hasSurrend=1:hands={bet=10.5f;cards=(0,2)(10,2);value=10}{bet=100.5;cards=(10,2)(3,3)(2,1)}:earnings=10.5][name=Croupier:position=7:money=100.5:"
				+"isPlaying=1:hasSurrend=1:hands={bet=10.5f;cards=(10,2)(9,2)}{bet=20;cards=(10,2)(3,3)(2,1)}]|actualSit=7|actualHand=0|yours=6|startCounter=10\n";*/
		if(s != null){
			if(s.length() >= 13 && s.substring(0,13).equals("GET /game/bet")){
				panGame.showActions(false);
				panGame.showBet(true);
				
			} else if(s.length() >= 25 && s.substring(0,25).equals("POST /game/disconnection/")){
				/* POST /game/disconnection/5\nhost:adress\nuser-agent:test\ncontent-length:10\n\n[reason=smtg]\n  */
				int seat = -1;
				String [] d = s.split("[ ]");
				String pos[] = d[1].split("[/]");
				seat = Integer.valueOf(pos[3]);
				String content[] = s.split("[\\[\\]]");
				String msg = content[1].split("[=]")[1];
				panGame.removePlyr(seat,msg);
				
			
			} else if(s.length() >= 21 && s.substring(0,21).equals("HTTP/1.1 202 ACCEPTED")){
				/* HTTP/1.1 202 ACCEPTED */
				panGame.showBet(false);
				panGame.showActions(true);
				panGame.enableButtons(false,false,false,false,false);
				
		    } else if(s.length() >= 21 && s.substring(0,21).equals("HTTP/1.1 403 FORBIDDEN")){
		    	panGame.showWrongBet();
		    } else if( s.length() >= 20 && s.substring(0,20).equals("GET /game/action/ask")){
		    	panGame.canAnswer(true);
		    } else if(s.length() >= 26 && s.substring(0,26).equals("POST /game/action/possible")){
		    	 // POST /game/action/possible\nh; [:::]
		    	 boolean[] res;
		    	 String base[] = s.split("[\\[\\]]");
		    	 String vals[] = base[1].split("[:]");
		    	 
		    	 res = new boolean[5];
		    	 for(int i=0;i<5;i++){
		    		 if(vals[i].split("[=]")[1].equals("1")){
		    			 res[i] = true;
		    		 } else {
		    			 res[i] = false;
		    		 }
		    	 }
		    	 
		    	 if(panGame.getTimerAct() != null){
		    		 try {
						panGame.getTimerAct().join();
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
		    	 }
		    	 panGame.startTimerAct();
		    	 
		    	 if(!isIA){
		    		 panGame.enableButtons(res[0],res[1],res[2],res[3],res[4]);
		    	 } else {
		    		 try {
						Thread.sleep(1000);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
		    		 int action = 0;
		    		 ArrayList<Integer> dispo = new ArrayList<Integer>();
		    		 for(int i=0;i<res.length;i++){
		    			 if(res[i] == true){
		    				 dispo.add(new Integer(i));
		    			 }
		    		 }
					Random r= new Random();
					int x = r.nextInt(dispo.size());
					action = dispo.get(x);
					
		    		 switch(action){
		    		 case 0:
		    			 panGame.hit();
		    			 break;
		    		 case 1:
		    			 panGame.stand();
		    			 break;
		    		 case 2:
		    			 panGame.myDouble();
		    			 break;
		    		 case 3:
		    			 panGame.surrend();
		    			 break;
		    		 case 4:
		    			 panGame.split();
		    			 break;
		    			 default:
		    				 panGame.stand();
		    				 break;
		    		 }
		    	 }
		    	 
		    } else if(s.length() >= 22 && s.substring(0,22).equals("POST /game/actual-seat")){
		    	/* 5 lignes */
		    	int pos = Integer.valueOf(s.substring(23,24));
		    	panGame.upCurrentPly(pos);
			} else if(s.length() >= 19 && s.substring(0,19).equals("POST /game/earnings")){
				String ply[] = s.split("[\\[\\]]");
				if(ply.length > 1 && !ply[1].equals("")){
					String name,hasSurrend;
					float earng;
					boolean surrend=true;
					for(int i=1;i<ply.length;i++){
						if(ply[i] != null && !ply[i].equals("")){
							String [] datas = ply[i].split("[:]");
							name = datas[0].split("[=]")[1];
							earng = Float.valueOf(datas[1].split("[=]")[1]);
							hasSurrend = datas[2].split("[=]")[1];
							if(name.equals(panGame.getMyName())){
								panGame.addMoney(earng);
							}
							if(hasSurrend.equals("1")){
								surrend =true;
							} else {
								surrend=false;
							}
							panGame.addEarning(name,earng,surrend);
						}
					}
					
					if(surrend == false){
						panGame.showActions(false);
					}
				} else {
					panGame.showActions(false);
				}

			} else if(s.length() >= 16 && s.substring(0,16).equals("POST /game/start")){
				String data[] = s.split("[\\[\\]]");
				int startCounter = Integer.valueOf(data[1].split("[=]")[1]);
				PanGame.TimerStart timer = panGame.new TimerStart(startCounter);
				timer.start();
				
			} else if(s.length() >= 4 && s.substring(0,4).equals("ping")){
				/* ne rien faire */
			
			} else if(s.length() >= 18 && s.substring(0,18).equals("POST /game/message")){
				/* POST /game/message */
				String data[] = s.split("[\\[\\]]");
				String msg = data[1].split("[=]")[1];
				panGame.addMsg(msg);
			
			} else {
				String split1[] = s.split("[ ]");
				if(split1.length > 2 && (split1[1].equals("/game/infos-player") || split1[1].substring(0,split1[1].length()-1).equals("/game/infos-player/") || split1[1].equals("/game/init")
						|| split1[1].equals("/game/firstDistribution") )){
					
						String [] names = new String[8];
						int seat1 = -1;
						for(int i=0;i<names.length;i++){
							names[i]=null;
						}
						float wallet=0f;
						int curr[],myPos;
						float bets[][];
						int values[][];
						float []earnings = new float[7];
						boolean surrenders[]= new boolean[]{false,false,false,false,false,false,false};
						for(int i=0;i<earnings.length;i++){
							earnings[i]=-1;
						}
						values = new int[8][3];
						for(int i=0;i<8;i++){
							values[i][0] =0;
							values[i][1] =0;
							values[i][2] =0;
						}
						bets = new float[8][3];
						ArrayList <int[]> allcard[][] = new ArrayList[8][3];
						for(int i=0;i<8;i++){
							for(int j=0;j<3;j++){
								allcard[i][j] = new ArrayList<int[]>();
							}
						}
						
						curr = new int[2];
						String datas0[] = s.split("[|]");
						String actualSit[] = datas0[1].split("[=]");
						curr[0] = Integer.valueOf(actualSit[1]);
						String actualHand[] = datas0[2].split("[=]");
						curr[1]= Integer.valueOf(actualHand[1]);
						String yours[] = datas0[3].split("[=]");
						myPos = Integer.valueOf(yours[1]);
						
						String plyrs[] = datas0[0].split("[\\[\\]]");
						for(int i=1;i<plyrs.length;i++){
							if(!plyrs[i].equals("") && plyrs[i] != null){
								String datas[] = plyrs[i].split("[:]");
								String name[] = datas[0].split("[=]");
								String pos[] = datas[1].split("[=]");
								seat1 = Integer.valueOf(pos[1]);
								if(!pos[1].equals("7")) names[Integer.valueOf(pos[1])] = name[1];
								if(!pos[1].equals("7")) earnings[Integer.valueOf(pos[1])] = Float.valueOf(datas[6].split("[=]")[1]);
								String money[] = datas[2].split("[=]");
								if(Integer.valueOf(pos[1]) == myPos) wallet = Float.valueOf(money[1]);
								String play[] = datas[3].split("[=]");
								String hasSurrend[] = datas[4].split("[=]");
								if(!pos[1].equals("7") && hasSurrend[1].equals("1")){
									surrenders[Integer.valueOf(pos[1])]=true;
								}
								String hands[] = datas[5].split("[{}]"); // ne pas prendre la 1ere 
								int posH=0;
								for(int j=1;j<hands.length;j++){
									if(!hands[j].equals("") && hands[j] != null){
										String datas2[] = hands[j].split("[;]");
										String bet[] = datas2[0].split("[=]");
										if(!pos[1].equals("7")) bets[Integer.valueOf(pos[1])][posH]=Float.valueOf(bet[1]);
										String cards[] = datas2[1].split("[()]");
										int value = Integer.valueOf(datas2[2].split("[=]")[1]);
										values[Integer.valueOf(pos[1])][posH]=value;
										for(int k=1;k<cards.length;k++){
											if(!cards[k].equals("") && cards[k] != null){
												String colorValue[] = cards[k].split("[,]");
												int[] cv ={Integer.valueOf(colorValue[1]),Integer.valueOf(colorValue[0])};
												allcard[Integer.valueOf(pos[1])][posH].add(cv);
											}
										}
										posH++;
									}
								}
							}
						}
						if(split1.length > 2 && split1[1].equals("/game/init")){
							willingQuit = false;
							frame.showGame(isIA);
							panGame.init(names,wallet,curr,myPos,bets,allcard,values,earnings,surrenders);
						} else if(split1.length > 2 && split1[1].equals("/game/infos-player")){
							panGame.init(names,wallet,curr,myPos,bets,allcard,values,earnings,surrenders);
						} else if(split1.length > 2  && split1[1].equals("/game/firstDistribution")){
							panGame.distribute(allcard,curr,values);
						} else {
							if(seat1 != -1){
								panGame.updateAmove(seat1,names[seat1],bets[seat1],allcard[seat1],wallet,values[seat1],curr[1]);
							}
						}
					
				}
			}
		}
	}
	
	private class ReceiveTCP extends Thread  {
		private boolean canRunRtcp = true;
		
		
		public void run(){
			BufferedReader socketReader=null;
			try {
				socketReader = new BufferedReader(new InputStreamReader(socktcp.getInputStream()));
			} catch (IOException e) {
				e.printStackTrace();
			}
			while(canRunRtcp){
				try {
					String serversMsg = "";
					String line;
					int inc = 0;
					boolean isget = false;
					boolean isHttp = false;
					boolean isPing = false;
					while((line = socketReader.readLine()) != null){
						if(inc == 0 && line.length() >= 3 && line.substring(0,3).equals("GET")){
							isget = true;
						} else if(inc == 0 && line.length() >= 4 && line.substring(0,4).equals("HTTP")) {
							isHttp =true;
						} else if(inc == 0 && line.length() >= 4 && line.substring(0,4).equals("ping")){
							isPing =true;
						}

						serversMsg+=line;
						inc+=1;
						if((inc==1 && isPing) || inc>= 6 || (inc>=5 && (isget || isHttp))){
							break;
						}
					}
					
					if(serversMsg == ""){
						/* si le serveur n'est plus dispo */
						canRunRtcp = false;
						frame.showMenu("bet");
						if(panGame != null){
							panGame.stopThreads();
						}
					} else {
						if(!serversMsg.equals("ping")){
							//System.out.println("LE MSG: "+serversMsg);
						}
						parse(serversMsg);
					}
				} catch (IOException e2) {
					canRunRtcp = false;
				}
			}
			if(!willingQuit){
				ClosingtcpThread closetcp = new ClosingtcpThread();
				closetcp.start();
			}
			if(panGame != null){
				panGame.stopThreads();
			}
			
			if(!willingQuit){
				if(panGame.getMsgDisco() != null && !panGame.getMsgDisco().equals("")){
					frame.showMenu(panGame.getMsgDisco());	
				} else {
					frame.showMenu("general");	
				}
			} else {
				frame.showMenu("update");
			}
			
		}
		
		public void canRunRtcp(boolean b){
			canRunRtcp = b;
		}
	}
	
	public void canRuntcp(boolean b){
		recTCP.canRunRtcp(b);
	}
	
	public ArrayList<String[]> wantToPlay(){
    		servers = new ArrayList<String[]>();
			try {
		        InetAddress addr = InetAddress.getByName("255.255.255.255");

		        DatagramSocket serverSocket = new DatagramSocket();
		        
                String msg = "I WANT TO PLAY ANDNIC";
                
		        UDPreceived udpRec= new UDPreceived();
		        udpRec.start();

		        
		        DatagramPacket msgPacket = new DatagramPacket(msg.getBytes(),msg.getBytes().length, addr,2301);
	            serverSocket.send(msgPacket);
		        Thread.sleep(1000);
	            udpRec.canRun(false);
	            udpRec.join();
		        serverSocket.close();
		        
		        
			} catch (UnknownHostException e) {
				e.printStackTrace();
			} catch (IOException e){
                    e.printStackTrace();
               } catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			return servers;
	}
	
	
	public class UDPreceived extends Thread {
		private boolean canRun = true;
	 
	    public void run() {
	    	DatagramSocket dsocket = null;
	    	try {
	    	      int port = 2303;
	    	      dsocket = new DatagramSocket(port);
	    	      dsocket.setSoTimeout(1000);
	    	      dsocket.setReuseAddress(true);
	    	      byte[] buffer = new byte[2048];
	    	      DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
	    	      while (canRun) {
	    	        dsocket.receive(packet);
	    	        String msg = new String(buffer, 0, packet.getLength());
	    	        //System.out.println("le msg:"+msg);
	    	        
	    	        /* analyse come here to have fun */
	    			String[] strInit = msg.split("[-]");
	    			if(strInit.length > 0 && strInit[0].equals("COME HERE TO HAVE FUN ")){
	    				String[] splitStr = strInit[1].split("[{}]");
	    				String[] datas = splitStr[1].split("[:]");
	    				String[] address = splitStr[0].split(":");
	    				address[0] = address[0].replace(" ","");
	    				String[] finalDatas = new String[7];
	    				int i;
	    				for(i=0;i<7;i++){
	    					if(i<2){
	    						finalDatas[i] = address[i];
	    					} else {
	    						finalDatas[i] = datas[i-2];
	    					}
	    				}
	    				
	    				boolean canAdd = true;
	    				for(String[]server:servers){
	    					if(server[0].equals(finalDatas[0]) && server[1].equals(finalDatas[1])){
	    						canAdd = false;
	    						break;
	    					}
	    				}
	    				if(canAdd){
	    					servers.add(finalDatas);
	    				}
	    				
	    			}
	    			
	    	        packet.setLength(buffer.length);
	    	      }
	    	      dsocket.close();
	    		} catch (SocketTimeoutException e){
	    			dsocket.close();
	    	    } catch (Exception e) {
	    	      System.err.println(e);
	    	    }
	    }
	    
	    public void canRun(boolean b){
	    	canRun =b;
	    }
	}

	public String makeMsg(String type, String url, int len,String data,boolean datas) { /* type= POST content lenght = 0  URL= /game/action */
		String w = type+" "+url+" HTTP/1.1\nhost: "+theAddress+":"+thePort+"\ncontent-length: "+len+"\n\n";
		if(datas){
			w+=data;
		}
		return w;
	}

	public void setPanGame(PanGame panGame) {
		this.panGame = panGame;
	}

	public void willingQuit(boolean b) {
		willingQuit = b;
	}

	public void setIA(boolean isIA) {
		this.isIA = isIA;
	}
}
