import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.awt.geom.RoundRectangle2D;
import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Random;

import javax.swing.BorderFactory;
import javax.swing.JComponent;
import javax.swing.JFormattedTextField;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;

public class PanGame  extends JPanel implements MouseWheelListener {
	private Frame frame;
	private String originalName;
	private Network myNw;
	private Button btnHit;
	private Button btnStand;
	private Button btnDouble;
	private Button btnSurrender;
	private Button btnSplit;
	private Button btnQuit;
	private Button btnBet;
	private int sizeName;
	private int sizeNames[];
	private int sizeBets[];
	private int sizeWallet;
	private CardButton[][] cardsPlayers;
	private int[] theSelectedOne;
	private String[] names;
	private int mySit;
	private float myWallet;
	private float bets[][];
	private int posSit[];
	private int theCurrent[];
	private int posRec[][];
	private int values[][][]; /* [][][1] = 1 si Black Jack */
	private boolean canDraw;
	private float theBet;
	private float limitBet[];
	private JFormattedTextField betInput;
	private JLabel limitBetJlb;
	private JLabel incorrectBet;
	private JLabel timeLeft;
	private JLabel waitingPlyJlb;
	private boolean myTurn;
	private JLabel actionTimeJlb;
	private boolean answer;
	private String msg;
	private TimerAct timAct;
	private boolean fin;
	private boolean actionEnable[]; // hit stand split double surrend 
	private JTextArea contentMsg;
	private boolean isIA;
	private String msgDisco;

	
	public PanGame(int w, int h,Frame frm,Network mynw, boolean isIA){
		/* TEST TEST */
		limitBet = new float[]{5,100};
		/* FIN TEST FIN TEST */
		this.isIA = isIA;
		actionEnable = new boolean[]{false,false,false,false,false};
		
		this.frame = frm;
		this.setPreferredSize(new Dimension(w,h));
		this.setBackground(Color.WHITE);
		this.setLayout(null);
	    setFocusable(true);
	    this.myNw = mynw;
	    canDraw = false;
	    
	    contentMsg = new JTextArea("");
	    contentMsg.setForeground(Color.WHITE);
	    contentMsg.setEditable(false);
	    contentMsg.setFont(Ressources.font.deriveFont(12.f));
	    contentMsg.setBackground(new Color(0,0,0,1));
	    contentMsg.setWrapStyleWord(true);
	    contentMsg.setLineWrap(true);
	    contentMsg.setHighlighter(null);
	    contentMsg.setBounds(730,10,320,150);
	    add(contentMsg);
	    
	     
	    limitBetJlb = new JLabel("€  (Entre "+limitBet[0]+" et "+limitBet[1]+")");
	    limitBetJlb.setFont(Ressources.font.deriveFont(17.f));
	    limitBetJlb.setBounds(555,h-90,200,20);
	    add(limitBetJlb);
	    limitBetJlb.setVisible(false);
	    
	    actionTimeJlb = new JLabel("Vous avez 10s pour répondre.");
	    actionTimeJlb.setFont(Ressources.font.deriveFont(17.f));
	    actionTimeJlb.setForeground(Color.WHITE);
	    actionTimeJlb.setBounds(785,h-145,280,20);
	    add(actionTimeJlb);
	    actionTimeJlb.setVisible(false);
	    
	    timeLeft = new JLabel("Vous avez 10s pour miser.");
	    timeLeft.setFont(Ressources.font.deriveFont(14.f));
	    timeLeft.setBounds(835,h-90,200,20);
	    add(timeLeft);
	    timeLeft.setVisible(false);
	    
	    waitingPlyJlb = new JLabel("La partie commence dans 10s.");
	    waitingPlyJlb.setFont(Ressources.font.deriveFont(14.f));
	    waitingPlyJlb.setBounds(805,h-90,300,20);
	    add(waitingPlyJlb);
	    waitingPlyJlb.setVisible(false);
	    
	    incorrectBet = new JLabel("Mise incorrecte !");
	    incorrectBet.setFont(Ressources.font.deriveFont(17.f));
	    incorrectBet.setForeground(Color.RED);
	    incorrectBet.setBounds(560,h-37,200,20);
	    add(incorrectBet);
	    incorrectBet.setVisible(false);
	    
	    btnHit = new Button("Tirer",24.f,322,h-100,230);
	    btnStand = new Button("Rester",24.f,322,h-50,230);
	    btnDouble = new Button("Doubler",24.f,570,h-100,230);
	    btnSurrender = new Button("Abandonner",24.f,820,h-100,230);
	    btnSplit = new Button("Diviser",24.f,570,h-50,230);
	    btnQuit = new Button("Quitter le serveur",24.f,820,h-50,230);
	    btnBet = new Button("Miser",24.f,322,h-50,230);
	    
	    btnHit.setVisible(false);
	    btnStand.setVisible(false);
	    btnDouble.setVisible(false);
	    btnSurrender.setVisible(false);
	    btnSplit.setVisible(false);
	    btnQuit.setVisible(true);
	    btnBet.setVisible(false);
	    
	    betInput = new JFormattedTextField();
	    betInput.setVisible(false);
	    betInput.setHorizontalAlignment(JTextField.CENTER);
	    betInput.setBounds(322,h-100,230,40);
	    betInput.setFont(Ressources.font.deriveFont(20.f));
	    this.add(betInput);
	    
	    btnHit.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e)  {
				if(actionEnable[0]){
					hit();
				}
			}
		});
	    
	    btnStand.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e)  {
				if(actionEnable[1]){
					stand();
				}
			}
		});
	    
	    btnDouble.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e)  {
				if(actionEnable[3]){
					myDouble();
				}
			}
		});
	    
	    btnSurrender.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e)  {
				if(actionEnable[4]){
					surrend();
				}
			}
		});
	    
	    btnSplit.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e)  {
				if(actionEnable[2]){
					split();
				}
			}
		});
	    
	    btnQuit.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e)  {
				myNw.willingQuit(true);
				Network.ClosingtcpThread close = myNw.new ClosingtcpThread();
				close.start();
				try {
					close.join();
				} catch (InterruptedException e1) {
					e1.printStackTrace();
				}
				fin = true;
				answer = true;
			}
		});
	    
	    btnBet.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e)  {
				if(betInput.getText() != null && !betInput.getText().equals("")){
					theBet = -1;
					try{
						theBet = Float.valueOf(betInput.getText());
					} catch(NumberFormatException n){
						
					}
					if(theBet != -1 && theBet >= limitBet[0] && theBet <= limitBet[1] && theBet<=myWallet){
						bet(theBet);
					} else {
						incorrectBet.setVisible(true);
					}
				} else {
					incorrectBet.setVisible(true);
				}
			}
		});
	    
	    this.add(btnHit);
	    this.add(btnStand);
	    this.add(btnDouble);
	    this.add(btnSurrender);
	    this.add(btnQuit);
	    this.add(btnSplit);
	    this.add(btnBet);
	    
	    
	    cardsPlayers = new CardButton [8][3];
	    theSelectedOne = new int[2];
	    theSelectedOne[0] = -1;
	    theSelectedOne[1] = -1;
	    
	    names = new String[7];
	    bets = new float[7][3];
	    for(int i=0;i<7;i++){
	    	for(int j=0;j<3;j++){
	    		bets[i][j] = 0f;
	    	}
	    }
	    
	    sizeNames = new int[7];
		sizeBets = new int[7];
		posSit= new int[]{39,195,352,504,655,804,954};
		
		posRec = new int[][]{
			{35,411},
			{191,408},
			{348,408},
			{500,407},
			{651,406},
			{800,405},
			{950,404},
			{8,8}};
			
			
		values = new int[8][3][2];
		for(int i=0;i<8;i++){
			for(int j=0;j<3;j++){
				values[i][j][0]=0;
				values[i][j][1]=0;
			}
		}
	    
	    addMouseWheelListener(this);
	}
	
	public void hit(){
		if(myTurn){
			String msg = "action=hit";
			myTurn =false;
			if(myNw.sendTCP("",0,myNw.makeMsg("POST","/game/action/"+mySit,msg.length(),msg,true))== -1){
				exit(true,"general");
			}
			answer = true;
		}
	}
	
	public void stand(){
		if(myTurn){
			String msg = "action=stand";
			myTurn =false;
			if(myNw.sendTCP("",0,myNw.makeMsg("POST","/game/action/"+mySit,msg.length(),msg,true))== -1){
				exit(true,"general");
			}
			answer = true;
		}
	}
	
	public void myDouble(){
		if(myTurn){
			String msg = "action=double";
			myTurn =false;
			if(myNw.sendTCP("",0,myNw.makeMsg("POST","/game/action/"+mySit,msg.length(),msg,true)) == -1){
				exit(true,"general");
			}
			answer = true;
		}
	}
	
	public void surrend(){
		if(myTurn){
			myTurn =false;
			String msg = "action=surrend";
			if(myNw.sendTCP("",0,myNw.makeMsg("POST","/game/action/"+mySit,msg.length(),msg,true)) == -1){
				exit(true,"general");
			}
			answer = true;
		}
	}
	
	public void split(){
		if(myTurn){
			String msg = "action=split";
			myTurn =false;
			if(myNw.sendTCP("",0,myNw.makeMsg("POST","/game/action/"+mySit,msg.length(),msg,true)) == -1){
				exit(true,"general");
			}
			answer = true;
		}
	}
	
	private void bet(float b) {
		theBet = Config.round(b,2);
		String msg = "bet="+theBet;
		if(myNw.sendTCP("",0,myNw.makeMsg("POST","/game/bet/"+mySit,msg.length(),msg,true)) == -1){
			exit(true,"general");
		}
	}
	
	public void showActions(boolean state){
		if(!isIA){
			if(state){
				incorrectBet.setVisible(false);
			}
		    btnHit.setVisible(state);
		    btnStand.setVisible(state);
		    btnDouble.setVisible(state);
		    btnSurrender.setVisible(state);
		    btnSplit.setVisible(state);
		    revalidate();
		    repaint();
		}
	}
	
	public void showBet(boolean state){
		if(!isIA){
			if(state){
			    TimerBet tbet = new TimerBet();
			    tbet.start();
			    waitingPlyJlb.setVisible(false);
			    btnQuit.setVisible(true);
			}
			
			btnBet.setVisible(state);
		    betInput.setVisible(state);
		    limitBetJlb.setVisible(state);
		    timeLeft.setVisible(state);
		} else {
			if(state){
				waitingPlyJlb.setVisible(false);
				float min = limitBet[0];
				float max = Math.min(limitBet[1],myWallet);
				Random x= new Random();
				float b = Config.round(min+x.nextFloat()*(max-min),2);
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
				bet(b);
			}
		}
	    
	}
	
	public void updateAmove(int seat,String name,float[] bets2, ArrayList<int[]>[] allcard,float wallet, int[] vals,int actualHand){
		if(seat != 7 ) names[seat] = name;
		if(seat == mySit){
			myWallet = Config.round(wallet,2);
		}
		theCurrent[1] = actualHand;
	  
    	for(int j=0;j<3;j++){
    		if(seat!= 7) bets[seat][j]=Config.round(bets2[j],2);
    		if(cardsPlayers[seat][j] == null){
    			cardsPlayers[seat][j] = new CardButton(this,j,seat);
    			this.add(cardsPlayers[seat][j]);
    		} else {
    			cardsPlayers[seat][j].removeCards();
    		}
    	    for(int k=0;k<allcard[j].size();k++){
    	    	if(seat==7 && (j==1 || j==2)) break;
    	    	cardsPlayers[seat][j].addCard(allcard[j].get(k)[0],allcard[j].get(k)[1]);
    	    }
    	    if(seat!= 7 || (seat==7 && j== 0)) values[seat][j][0]=vals[j];
    	    if(allcard[j].size() == 2 && vals[j] == 21 && (cardsPlayers[seat][1]==null || cardsPlayers[seat][1].getNb() == 0 )){
    	    	values[seat][j][1]=1;
    	    } else {
    	    	values[seat][j][1]=0;
    	    }
    	}
	    

    	if(seat != 7){
	    	if(names[seat] != null){
	    		sizeNames[seat] = Config.sizeOfaString(names[seat],Ressources.font.deriveFont(18.f));
				if(bets[seat][0]%(int)bets[seat][0] == 0){
					sizeBets[seat] = Config.sizeOfaString(""+(int)bets[seat][0],Ressources.font.deriveFont(14.f))+18;
				} else {
					sizeBets[seat] = Config.sizeOfaString(""+bets[seat][0],Ressources.font.deriveFont(14.f))+18;
				}
	    	} else {
	    		sizeNames[seat] = 0;
	    	}
    	}
    	revalidate();
    	repaint();
	   
	}
	
	public void removePlyr(int seat, String msg){
		if(mySit == seat){
			exit(true,msg);
	
		} else {
			if(seat == theSelectedOne[0]){
				theSelectedOne[0] = -1;
			}
			names[seat] = null;
			bets[seat][0]=0f;
			bets[seat][1]=0f;
			bets[seat][2]=0f;
			for(int i=0;i<3;i++){
				if(cardsPlayers[seat][i] != null){
					remove(cardsPlayers[seat][i]);
					cardsPlayers[seat][i] = null;
				}
			}
			for(int i=0;i<3;i++){
				values[seat][i][0] = 0;
				values[seat][i][1] = 0;
			}
			repaint();
		}
		/* POST /game/disconnection/1  */
	}
	
	public void exit(boolean showMenu,String msg){
		Network.ClosingtcpThread close = myNw.new ClosingtcpThread();
		close.start();
		try {
			close.join();
		} catch (InterruptedException e1) {
			e1.printStackTrace();
		}
		fin = true;
		answer = true;
		if(showMenu){
			msgDisco = msg;
			frame.showMenu(msg);
		}
	}
	
	public void init(String[] names2, float wallet, int[] curr, int myPos, float[][] bets2, ArrayList<int[]>[][] allcard, int[][] vals, float[] earnings2, boolean[] surrenders){

		theCurrent = new int[]{curr[0],curr[1]};
		msg = "";
		contentMsg.setText(msg);
	    mySit = myPos;
	    myWallet = Config.round(wallet,2);
		
		for(int i=0;i<names2.length-1;i++){
			names[i] = names2[i];
		}
		
		for(int i=0;i<earnings2.length;i++){
			if(earnings2[i] != -1){
				addEarning(names[i],earnings2[i],surrenders[i]);
			}
		}

	    
	    for(int i=0;i<8;i++){
	    	for(int j=0;j<3;j++){
	    		if(i!= 7) bets[i][j]=Config.round(bets2[i][j],2);
	    		if(cardsPlayers[i][j] == null){
		    		cardsPlayers[i][j] = new CardButton(this,j,i);
	    			this.add(cardsPlayers[i][j]);
	    		} else {
	    			cardsPlayers[i][j].removeCards();
	    		}
	    	    for(int k=0;k<allcard[i][j].size();k++){
	    	    	if(i==7 && (j==1 || j==2)) break;
	    	    	cardsPlayers[i][j].addCard(allcard[i][j].get(k)[0],allcard[i][j].get(k)[1]);
	    	    }
	    	    if(i!= 7 || (i==7 && j== 0)) values[i][j][0]=vals[i][j];
	    	    if(allcard[i][j].size() == 2 && vals[i][j] == 21 && (cardsPlayers[i][1]==null || cardsPlayers[i][1].getNb() == 0 )){
	    	    	values[i][j][1]=1;
	    	    } else {
	    	    	values[i][j][1]=0;
	    	    }
	    	}
	    }
	    
	    // FIN DES TEST
	    
	    sizeName = Config.sizeOfaString(names[mySit],Ressources.font.deriveFont(24.f));
	    sizeWallet = Config.sizeOfaString(myWallet+" €",Ressources.font.deriveFont(24.f));
	    for(int i=0;i<7;i++){
	    	if(names[i] != null){
	    		sizeNames[i] = Config.sizeOfaString(names[i],Ressources.font.deriveFont(18.f));
				if(bets[i][0]%(int)bets[i][0] == 0){
					sizeBets[i] = Config.sizeOfaString(""+(int)bets[i][0],Ressources.font.deriveFont(14.f))+18;
				} else {
					sizeBets[i] = Config.sizeOfaString(""+bets[i][0],Ressources.font.deriveFont(14.f))+18;
				}
	    	} else {
	    		sizeNames[i] = 0;
	    	}
	    }
	    canDraw = true;
	    revalidate();
	    repaint();
	
	}
	
	@Override
	public void paintComponent(Graphics g) {
		if(canDraw){
			Graphics2D g2 = (Graphics2D) g;
			g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING,RenderingHints.VALUE_ANTIALIAS_ON);
			g2.drawImage(Ressources.bgGame, 0, 0, null);
			g2.drawImage(Ressources.footer,0,690, null);
			g2.drawImage(Ressources.bar,0,680, null);
			g2.setColor(new Color(254,193,81,60));
			g2.fillRoundRect(10,700,300,93,15,15);
		
			g2.setFont(Ressources.font.deriveFont(18.f));
			g2.setColor(Color.BLACK);
			
			g2.drawString("Le croupier",7,178);
			
			int x=35;
			for(int i=0;i<names.length;i++){
				if(names[i] != null){
					g2.setFont(Ressources.font.deriveFont(18.f));
					if(i == mySit){
						g2.setColor(new Color(226,167,52));
					}
					g2.drawString(names[i],posSit[6-i]+(97-sizeNames[i])/2,580);
					g2.setColor(Color.BLACK);
					if(bets[i][0] != 0){
						g2.setFont(Ressources.font.deriveFont(14.f));
						int bet;
						if(bets[i][0]%(int)bets[i][0] == 0){
							g2.drawString(""+(int)bets[i][0],posSit[6-i]+(97-sizeBets[i])/2,605);
						} else {
							g2.drawString(""+bets[i][0],posSit[6-i]+(97-sizeBets[i])/2,605);
						}
						g2.drawImage(Ressources.tken,(posSit[6-i]+(97-sizeBets[i])/2)+sizeBets[i]-16,590, null);
					}
				}
				x+=160;
			}
			
			
			g2.setFont(Ressources.font.deriveFont(14.f));
			for(int i=0;i<7;i++){
				if(bets[i][1] != 0){
					if(bets[i][1] % (int)bets[i][1] == 0 ){
						g2.drawString(""+(int)bets[i][1],posRec[6-i][0]+(46-Config.sizeOfaString(""+(int)bets[i][1],Ressources.font.deriveFont(14.f)))/2,327);
					} else {
						g2.drawString(""+bets[i][1],posRec[6-i][0]+(46-Config.sizeOfaString(""+bets[i][1],Ressources.font.deriveFont(14.f)))/2,327);
					}
					g2.drawImage(Ressources.tken,posRec[6-i][0]+(46-18)/2,337, null);
				}
				if(bets[i][2] != 0){
					if(bets[i][2] % (int)bets[i][2] == 0){
						g2.drawString(""+(int)bets[i][2],posRec[6-i][0]+57+(46-Config.sizeOfaString(""+(int)bets[i][2],Ressources.font.deriveFont(14.f)))/2,327);
					} else {
						g2.drawString(""+bets[i][2],posRec[6-i][0]+57+(46-Config.sizeOfaString(""+bets[i][2],Ressources.font.deriveFont(14.f)))/2,327);
					}
					g2.drawImage(Ressources.tken,posRec[6-i][0]+57+(46-18)/2,337, null);
	
				}
			}
			
			g2.setFont(Ressources.font.deriveFont(18.f));
			for(int i=0;i<7;i++){
				for(int j=0;j<3;j++){
					if(values[i][j][0] != 0){
						if(j==0){
							if(values[i][j][1] == 1){
								g2.setColor(Color.RED);
							}
							g2.drawString(""+values[i][j][0],posSit[6-i]+(97-Config.sizeOfaString(""+values[i][j][0],Ressources.font.deriveFont(18.f)))/2,395);
							g2.setColor(Color.BLACK);
						} else if(j==1){
							g2.drawString(""+values[i][j][0],posSit[6-i]-4+(46-Config.sizeOfaString(""+values[i][j][0],Ressources.font.deriveFont(18.f)))/2,233);
						}else {
							g2.drawString(""+values[i][j][0],posSit[6-i]+52+(46-Config.sizeOfaString(""+values[i][j][0],Ressources.font.deriveFont(18.f)))/2,233);
						}
					}
				}
			}
			
			if(values[7][0][0] != 0){
				if(values[7][0][1] == 1){
					g2.setColor(Color.RED);
				}
				g2.drawString(""+values[7][0][0],120,30);
			}
			
			g2.setColor(new Color(226,167,52));
			g2.setFont(Ressources.font.deriveFont(24.f));
			g2.drawString(names[mySit],10+((300-sizeName)/2),700+30);
			g2.drawString(myWallet+" €",10+((300-sizeWallet)/2),700+80);
			
			
			if(theCurrent[0] != -1 && theCurrent[1] != -1){
				g2.setColor(Color.RED);
				RoundRectangle2D roundedRectangle;
				if(theCurrent[1] == 0){
					g2.setStroke(new BasicStroke(5.f));
					int p = 6- theCurrent[0];
					if(theCurrent[0] == 7){
						p = 7;
					}
					roundedRectangle = new RoundRectangle2D.Float(posRec[p][0],posRec[p][1], 102, 145, 10, 10);
				} else {
					g2.setStroke(new BasicStroke(3.f));
					int gap = 2;
					if(theCurrent[1] == 2) gap = -54;
					int p = 6- theCurrent[0];
					if(theCurrent[0] == 7){
						p = 7;
					}
					roundedRectangle = new RoundRectangle2D.Float(posRec[p][0]-gap,238,50,70, 10, 10);
				}
		        g2.draw(roundedRectangle);
			}
	        
		}
		
	}
	
	@Override
	public void mouseWheelMoved(MouseWheelEvent e) {
		if(theSelectedOne[0] != -1){
			int rotation = e.getWheelRotation();
			if (rotation < 0) {
				cardsPlayers[theSelectedOne[0]][theSelectedOne[1]].changeBgUp();
					
			} else {
				cardsPlayers[theSelectedOne[0]][theSelectedOne[1]].changeBgDown();
			}
		}
	}

	public void canRoll(int nSit, int nHand) {
		theSelectedOne[0] = nSit;
		theSelectedOne[1] = nHand;
	}

	public boolean isSelected(int nSit, int nHand) {
		return theSelectedOne[0] == nSit && nHand == theSelectedOne[1];
	}
	
	
	public class TimerBet extends Thread {
		public TimerBet(){
			
		}
		
		@Override
		public void run(){
			int i=9;
			while(i>0){
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
				timeLeft.setText("Vous avez "+i+"s pour miser.");
				i--;
			}
			
		}
	}

	public class TimerStart extends Thread {
		private int sec;
		public TimerStart(int i){
			sec=i;
		}
		
		@Override
		public void run(){
			
			waitingPlyJlb.setVisible(true);
			fin = false;
			while(sec>0 && !fin){
				waitingPlyJlb.setText("La partie commence dans "+sec+"s.");
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
				sec--;
			}
			
		}
	}
	
	
	public class TimerAct extends Thread {
		
		@Override
		public void run(){
			
			actionTimeJlb.setVisible(true);
			int sec=10;
			answer = false;
			while(sec>0 && !answer){
				actionTimeJlb.setText("Vous avez "+sec+"s pour répondre.");
				revalidate();
				repaint();
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
				sec--;
			}
			answer =false;
			actionTimeJlb.setVisible(false);
			
		}
	}

	public TimerAct getTimerAct(){
		return timAct;
	}
	
	public void showWrongBet() {
		incorrectBet.setVisible(true);
	}

	public void enableButtons(boolean enableHit, boolean enableStand, boolean enableDouble, boolean enableSurrend,boolean enableSplit) {
		actionEnable[0] = enableHit;
		actionEnable[1] = enableStand;
		actionEnable[2] = enableSplit;
		actionEnable[3] = enableDouble;
		actionEnable[4] = enableSurrend;
		
		btnHit.setEnabled(enableHit);
	    btnStand.setEnabled(enableStand);
	    btnDouble.setEnabled(enableDouble);
	    btnSurrender.setEnabled(enableSurrend);
	    btnSplit.setEnabled(enableSplit);
	    repaint();
	}

	public void distribute(ArrayList<int[]>[][] allcard,int curr[], int[][] values2) {
		int save;
	    for(int i=0;i<8;i++){
	    	save=i;
	    	if(i==0){
	    		i=7;
	    	} else {
	    		i=i-1;
	    	}

    		cardsPlayers[i][0] = new CardButton(this,0,i);
    	    this.add(cardsPlayers[i][0]);
    	    for(int k=0;k<allcard[i][0].size();k++){
    	    	cardsPlayers[i][0].addCard(allcard[i][0].get(k)[0],allcard[i][0].get(k)[1]);
        	    repaint();
    	    	try {
					Thread.sleep(500);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
    	    }
    	    values[i][0][0] = values2[i][0];
    	    
    	    if(allcard[i][0].size() == 2 && values2[i][0]==21 && (cardsPlayers[i][1]==null || cardsPlayers[i][1].getNb() == 0 )){
    	    	values[i][0][1] = 1;
    	    } else {
    	    	values[i][0][1] = 0;
    	    }
    	    repaint();
    	    i = save;
	    }
		/*theCurrent[0] = curr[0];
		theCurrent[1] = curr[1];*/
		repaint();
		myTurn = false;
	}

	public void canAnswer(boolean state) {
		myTurn = state;
	}

	public void upCurrentPly(int seat) {
		if(seat != mySit){
			enableButtons(false,false,false,false,false);
		}
		theCurrent[0]=seat;
		theCurrent[1] =0;
		repaint();
	}
	
	public void addEarning(String name,float earning,boolean hasSurrend){
		if(earning > 0){
			if(!hasSurrend){
				msg="* "+name+" a gagné "+Config.round(earning,2)+" € !\n"+msg;
				contentMsg.setText(msg);
			} else {
				msg="* "+name+" a abandonné et gagne "+Config.round(earning,2)+" € !\n"+msg;
				contentMsg.setText(msg);
			}
		} else {
			msg="* "+name+" n'a rien gagné !\n"+msg;
			contentMsg.setText(msg);
		}
		revalidate();
		repaint();
	}

	public String getMyName() {
		return names[mySit];
	}

	public void addMoney(float earng) {
		myWallet = Config.round(myWallet+=earng,2);
		repaint();
	}

	public void startTimerAct() {
		timAct = new TimerAct();
		timAct.start();
	}

	public void stopThreads() {
		answer = true;
		fin = true;
	}

	public void addMsg(String msg2) {
		/* ajoute message a la fenetre */
		msg = "* "+msg2+"\n" + msg;
		contentMsg.setText(msg);
		repaint();
	}

	public String getMsgDisco() {
		return msgDisco;
	}
}
