import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.ArrayList;

import javax.swing.JFrame;

@SuppressWarnings("serial")
public class Frame extends JFrame {
	private PanMenu panMenu;
	private PanGame panGame;
	private int width;
	private int height;
	private Network network;
	
	/***
	 * Constructeur de la Frame
	 * '
	 * @param w
	 * 	longueur de la fenêtre
	 * @param h
	 * 	largeur de la fenêtre
	 */
	public Frame(int w,int h) {
		network = new Network(this);
		width=w;
		height=h;
		this.setTitle("Blackjack");
		this.setSize(w, h);
		this.setResizable(false);
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

	    this.addWindowListener(new WindowAdapter(){
            public void windowClosing(WindowEvent e){
                  if(panGame != null){
                	  panGame.exit(false,"general");
                  }
            }
	    });
		
		
		panMenu = new PanMenu(w,h,this,network);
		
		this.setContentPane(panMenu);
		this.pack();
		this.setVisible(true);
		this.setLocationRelativeTo(null);
	}
	
	
	public void showGame(boolean isIA){
		panGame = new PanGame(width,height,this,network,isIA);
		network.setPanGame(panGame);
		this.setContentPane(panGame);
		this.pack();
		this.setVisible(true);
		//this.setLocationRelativeTo(null);
	}
	
	
	public void showMenu(String msg){
		this.setContentPane(panMenu);
		this.pack();
		this.setVisible(true);
		//this.setLocationRelativeTo(null);
		ArrayList<String[]> servers = new ArrayList<>();
		panMenu.showServers(servers);
		panMenu.setMsg(msg);
	}
	
}