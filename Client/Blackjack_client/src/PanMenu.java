import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.util.ArrayList;

import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JFormattedTextField;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

public class PanMenu extends JPanel {
	private Frame frame;
	private JFormattedTextField nameInput;
	private boolean invalidpseudo = false;
	private JLabel namejlb;
	private JLabel serverjlb;
	private Button btn;
	private JComboBox choiceServer;
	private ImageIcon hovered;
	private ImageIcon pressed;
	private ImageIcon arrow;
	private ImageIcon refreshHovered;
	private ImageIcon refresh;
	private ImageIcon refreshPressed;
	private JButton btnBack;
	private Network myNw;
	private Button btnOK;
	private JButton btnRefresh;
	private boolean refreshEnabled;
	private boolean wrongServer;
	private boolean canOK;
	private String myName;
	private int thePort;
	private String theAddress;
	private float posXmsg[]; // money,bet,general
	private int typeMsg;
	private String msgDisconnection="";
	private JCheckBox checkIA;
	private boolean isIA;

	
	/***
	 * Constructeur du PanMenu
	 * 
	 * @param w
	 * 		longueur du panel
	 * @param h
	 * 		hauteur du panel
	 * @param frm
	 * 		fenetre du panel
	 * 
	 */
	public PanMenu(int w, int h,Frame frm,Network myNw) {
		this.frame = frm;
		
		int []sizeMsg = new int[]{Config.sizeOfaString("Vous n'avez plus assez d'argent pour jouer.",Ressources.font.deriveFont(16.f)),Config.sizeOfaString("Vous n'avez pas parié à temps.",Ressources.font.deriveFont(16.f)),Config.sizeOfaString("Vous avez été déconnecté du serveur.",Ressources.font.deriveFont(16.f)),Config.sizeOfaString("Veuillez réactualiser la liste des serveurs.",Ressources.font.deriveFont(16.f))};
		posXmsg = new float[]{(w-sizeMsg[0])/2.0f,(w-sizeMsg[1])/2.0f,(w-sizeMsg[2])/2.0f,(w-sizeMsg[3])/2.0f};
		typeMsg = 3;
		msgDisconnection = " ";
		myName = "";
		this.setPreferredSize(new Dimension(w,h));
		this.setBackground(Color.WHITE);
		this.setLayout(null);
	    setFocusable(true);
	    this.myNw = myNw;
	    
		hovered = new ImageIcon(Ressources.arrowBackHovered);
		arrow = new ImageIcon(Ressources.arrowBack);
		pressed = new ImageIcon(Ressources.arrowBackPressed);
		
		refreshHovered = new ImageIcon(Ressources.refreshHovered);
		refresh = new ImageIcon(Ressources.refresh);
		refreshPressed = new ImageIcon(Ressources.refreshPressed);
		Config.canRefresh = true;
		showNameChoice();
		
	}
	
	public void setMyName(String n){
		myName = n;
		refreshEnabled = true;
	}
	
	public void showServers(ArrayList<String[]> servers){
		this.removeAll();
		invalidpseudo = false;
		wrongServer = false;
		
	    btnOK = new Button("OK",30.f,473,530,120);
	    btnOK.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e) {
				if(canOK){
					myName = myName.toLowerCase();
					myName = Character.toUpperCase(myName.charAt(0))+myName.substring(1,myName.length());

					if(myNw.openSocketTCP(theAddress,thePort)== -1){
						refreshEnabled=false;
						ArrayList<String[]> servers = myNw.wantToPlay();
						showServers(servers);
						refreshEnabled=true;
						typeMsg = 2;
						wrongServer = true;
						repaint();
					}else {
						myNw.setIA(isIA);
						myNw.listeningTCP();
						if(myNw.sendTCP(theAddress,thePort,myName) ==-1 ){
							refreshEnabled=false;
							ArrayList<String[]> servers = myNw.wantToPlay();
							showServers(servers);
							refreshEnabled=true;
							typeMsg = 2;
							wrongServer = true;
							repaint();
						}
					}
				}
			}
		});
	    this.add(btnOK);
		
	    serverjlb = new JLabel("Choisissez un serveur:");
	    serverjlb.setFont(Ressources.font.deriveFont(30.f));
	    serverjlb.setForeground(Color.WHITE);
	    serverjlb.setBounds(360,300,400,50);
		this.add(serverjlb);
		
		
		if(servers.size() != 0){
			String[] list = new String[servers.size()];
			for(int i=0;i<list.length;i++){
				list[i] = "<html>"+servers.get(i)[0]+":"+servers.get(i)[1]+"&nbsp;&nbsp;"+servers.get(i)[2]+"/7<br>Nombre de paquets: "+servers.get(i)[3]+"<br>Somme de départ: "+Config.round(Float.valueOf(servers.get(i)[4]),2)+"€<br>Mise: "+Config.round(Float.valueOf(servers.get(i)[5]),2)+"-"+Config.round(Float.valueOf(servers.get(i)[6]),2)+"€<br></html>";
			}
			theAddress=servers.get(0)[0];
			thePort= Integer.parseInt(servers.get(0)[1].replaceAll(" ",""));
			btnOK.setEnabled(true);
			canOK = true;
			choiceServer = new JComboBox(list);
			choiceServer.setSelectedIndex(0);
			choiceServer.setFont(getFont().deriveFont(20.f));
			choiceServer.addActionListener(new ActionListener() {
	
				@Override
				public void actionPerformed(ActionEvent e) {
					String res = (String)choiceServer.getSelectedItem();
					String[] tmp = res.split("[ ]");
					String[] fin = tmp[0].split("[:]");
					thePort = Integer.parseInt(fin[1]);
					theAddress = fin[0].substring(6,fin[0].length());;
				}
			});
			choiceServer.setBounds(380,370,300,120);
			this.add(choiceServer);
		} else {
			JLabel noServerjlb = new JLabel("Aucun serveur trouvé");
			btnOK.setEnabled(false);
			canOK = false;
			noServerjlb.setFont(Ressources.font.deriveFont(20.f));
			noServerjlb.setForeground(Color.WHITE);
			noServerjlb.setBounds(420,410,400,50);
			this.add(noServerjlb);
		}
		
		btnBack = new JButton("");
		btnBack.setBounds(20,20,58,36);
		btnBack.setFocusPainted(false);
		btnBack.setBorderPainted(false);
		btnBack.setContentAreaFilled(false);
		btnBack.setIcon(arrow);
		this.add(btnBack);
		btnBack.addMouseListener(new MouseListener() {
			
			@Override
			public void mouseReleased(MouseEvent e) {
				if(btnBack.contains(e.getX(), e.getY())){
					btnBack.setIcon(hovered);
				} else {
					btnBack.setIcon(arrow);	
				}
				
			}
			
			@Override
			public void mousePressed(MouseEvent e) {
				btnBack.setIcon(pressed);
			}
			
			@Override
			public void mouseExited(MouseEvent e) {
				btnBack.setIcon(arrow);
				
			}
			
			@Override
			public void mouseEntered(MouseEvent e) {
				btnBack.setIcon(hovered);
				
			}
			
			@Override
			public void mouseClicked(MouseEvent e) {
				showNameChoice();
				btnBack.setIcon(arrow);
			}
		});
		
		btnRefresh = new JButton("");
		btnRefresh.setBounds(690,402,54,54);
		btnRefresh.setFocusPainted(false);
		btnRefresh.setBorderPainted(false);
		btnRefresh.setContentAreaFilled(false);
		btnRefresh.setIcon(refresh);
		this.add(btnRefresh);
		btnRefresh.addMouseListener(new MouseListener() {
			
			@Override
			public void mouseReleased(MouseEvent e) {
				if(refreshEnabled){
					if(btnRefresh.contains(e.getX(), e.getY())){
						btnRefresh.setIcon(refreshHovered);
					} else {
						btnRefresh.setIcon(refresh);	
					}
				}
				
			}
			
			@Override
			public void mousePressed(MouseEvent e) {
				if(refreshEnabled){
					btnRefresh.setIcon(refreshPressed);
				}
			}
			
			@Override
			public void mouseExited(MouseEvent e) {
				if (refreshEnabled){
					btnRefresh.setIcon(refresh);
				}
			}
			
			@Override
			public void mouseEntered(MouseEvent e) {
				if(refreshEnabled){
					btnRefresh.setIcon(refreshHovered);
				}
			}
			
			@Override
			public void mouseClicked(MouseEvent e) {
				if(refreshEnabled){
					refreshEnabled = false;
					ArrayList<String[]> servers = myNw.wantToPlay();
					showServers(servers);
					btnRefresh.setIcon(refresh);
					refreshEnabled = true;
				}
			}
		});
		repaint();
	}
	
	public void showNameChoice(){
		isIA = false;
		myName = "";
		wrongServer = false;
		this.removeAll();
		
		JLabel ialbl = new JLabel("IA");
		ialbl.setBounds(680,385,100,30);
		add(ialbl);
		ialbl.setFont(Ressources.font.deriveFont(24.f));
		ialbl.setForeground(Color.WHITE);
		
	    checkIA = new JCheckBox("");
		checkIA.setOpaque(false);
		checkIA.addItemListener(new ItemListener() {
			
			@Override
			public void itemStateChanged(ItemEvent e) {
				isIA = true;
				nameInput.setEnabled(false);
				nameInput.setText("");
				myName = "IA";
				if (e.getStateChange() == ItemEvent.DESELECTED){
					myName = "";
					isIA = false;
					nameInput.setEnabled(true);
				}
				
			}
		});
		
		checkIA.setBounds(650,388,20,20);
		this.add(checkIA);
		invalidpseudo = false;
	    namejlb = new JLabel("Entrez votre pseudo:");
	    namejlb.setFont(Ressources.font.deriveFont(30.f));
	    namejlb.setForeground(Color.WHITE);
	    namejlb.setBounds(360,300,400,50);
		this.add(namejlb);
	    
	    
	    nameInput = new JFormattedTextField();
	    nameInput.setHorizontalAlignment(JTextField.CENTER);
	    nameInput.setBounds(455,370,150,60);
	    nameInput.setFont(Ressources.font.deriveFont(20.f));
	    this.add(nameInput);
	 
	    btn = new Button("Jouer",30.f,473,480,120);
	    btn.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e) {
				if(!myName.equals("IA") && (nameInput.getText().length() > 8 || nameInput.getText().length() < 4 || !nameInput.getText().matches("[a-zA-Z]+"))){
					invalidpseudo = true;
					repaint();
				} else {
					if(!myName.equals("IA")){
						myName = nameInput.getText();
					}
					/*myNw.openSocketTCP("172.31.129.122",2302); //172.31.129.122  192.168.1.12
					myNw.listeningTCP();
					myNw.sendTCP("172.31.129.122",2302,myName);*/
					ArrayList<String[]> servers = myNw.wantToPlay();
					refreshEnabled=true;
					showServers(servers);
				}
			}
		});
	    this.add(btn);
	    
	    repaint();
	}
	
	@Override
	public void paintComponent(Graphics g) {
		Graphics2D g2 = (Graphics2D) g;
		g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING,RenderingHints.VALUE_ANTIALIAS_ON);
		g2.drawImage(Ressources.bg, 0, 0, null);
		g2.setColor(Color.WHITE);
		g2.setFont(getFont().deriveFont(18.f));
		g2.drawString("© Nicolas Serf & Andréa Franco",390,790);
		if(invalidpseudo){
			g2.setColor(Color.RED);
			g2.setFont(Ressources.font.deriveFont(16.f));
			g2.drawString("Pseudo incorrect (4-8 LETTRES)",400,460);
		}
		if(wrongServer){
			g2.setColor(Color.RED);
			g2.setFont(Ressources.font.deriveFont(16.f));
			g2.drawString(msgDisconnection,posXmsg[typeMsg],515);
			
		}
		
	}

	public void setMsg(String msg) {
		if(msg.equals("bet")){
			typeMsg = 1;
			msgDisconnection = "Vous n'avez pas parié à temps." ;
			wrongServer = true;
		} else if(msg.equals("money")){
			typeMsg = 0;
			msgDisconnection = "Vous n'avez plus assez d'argent pour jouer.";
			wrongServer = true;
		} else if(msg.equals("general") || msg.equals("end") ){
			msgDisconnection = "Vous avez été déconnecté du serveur.";
			typeMsg =2;
			wrongServer = true;
		} else if(msg.equals("update")){
			msgDisconnection = "Veuillez réactualiser la liste des serveurs.";
			typeMsg =3;
			wrongServer = true;
		} else {
			wrongServer = false;
		}
		repaint();
	}
}