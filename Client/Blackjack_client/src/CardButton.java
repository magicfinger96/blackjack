import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;

import javax.swing.ImageIcon;
import javax.swing.JButton;

/***
 * Classe représentant les
 * boutons se trouvant au milieu
 * du plateau 
 * 
 * @author andrea
 *
 */
public class CardButton extends JButton {
	private int nHand;
	private int nSit;
	private int posX;
	private int posY;
	private int width;
	private int height;
	private boolean enabled;
	private PanGame panGame;
	int currentPos;
	ArrayList<int[]> cards;
	private Color colorPoint;
	
	/***
	 * Constructeur de la clasee
	 * 
	 * @param str
	 * 	phrase sur le bouton
	 * 
	 * @param sizeFont
	 * 	taille de la police
	 * 
	 * @param posX
	 * 	position du bouton
	 * @param posY
	 */
	public CardButton(PanGame pg,int nhand, int nsit) {
		panGame = pg;
		this.nHand = nhand;
		this.nSit = nsit;
		currentPos = 0;
		cards = new ArrayList<int[]>();
		colorPoint = Color.GREEN;
		
		switch(nSit){
		case 6:
			if(nhand ==0){
				posX=39;
				posY=415;
				width=95;
				height=139;
			} else if(nhand ==1){
				posX=35;
				posY=240;
				width=46;
				height=67;
			} else {
				posX=91;
				posY=240;
				width=46;
				height=67;
			}
			break;
		case 5:
			if(nhand ==0){
				posX=195;
				posY=412;
				width=95;
				height=139;
			} else if(nhand ==1){
				posX=191;
				posY=240;
				width=46;
				height=67;
			} else {
				posX=247;
				posY=240;
				width=46;
				height=67;
			}
			break;
		case 4:
			if(nhand ==0){
				posX=352;
				posY=412;
				width=95;
				height=139;
			} else if(nhand ==1){
				posX=348;
				posY=240;
				width=46;
				height=67;
			} else {
				posX=404;
				posY=240;
				width=46;
				height=67;
			}
			break;
		case 3:
			if(nhand ==0){
				posX=504;
				posY=411;
				width=95;
				height=139;
			} else if(nhand ==1){
				posX=500;
				posY=240;
				width=46;
				height=67;
			} else {
				posX=556;
				posY=240;
				width=46;
				height=67;
			}
			break;
		case 2:
			if(nhand ==0){
				posX=655;
				posY=410;
				width=95;
				height=139;
			} else if(nhand ==1){
				posX=651;
				posY=240;
				width=46;
				height=67;
			} else {
				posX=707;
				posY=240;
				width=46;
				height=67;
			}
			break;
		case 1:
			if(nhand ==0){
				posX=804;
				posY=409;
				width=95;
				height=139;
			} else if(nhand ==1){
				posX=800;
				posY=240;
				width=46;
				height=67;
			} else {
				posX=856;
				posY=240;
				width=46;
				height=67;
			}
			break;
		case 0:
			if(nhand ==0){
				posX=954;
				posY=408;
				width=95;
				height=139;
			} else if(nhand ==1){
				posX=950;
				posY=240;
				width=46;
				height=67;
			} else {
				posX=1006;
				posY=240;
				width=46;
				height=67;
			}
			break;
		case 7:
			if(nhand ==0){
				posX=12;
				posY=11;
				width=95;
				height=139;
			}
			break;
			default:
				break;
		}
		
		setBounds(posX,posY,width,height); // 95 139 ou 46 67
		setPreferredSize(new Dimension(width,height));
		
		setFocusPainted(false);
		setBorderPainted(false);
		setContentAreaFilled(false);
		
		addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e) {
				panGame.canRoll(nSit,nHand);
				
			}
		});
		
	}
	
	public void addCard(int color,int val){
		int c[] = {color,val};
		cards.add(c);
		
		if(!panGame.isSelected(nSit,nHand) || getIcon() == null){
			currentPos = cards.size()-1;
			ImageIcon bg;
			if(width == 95){
				bg = new ImageIcon(Ressources.cards[cards.get(currentPos)[0]][cards.get(currentPos)[1]]);
			} else {
				bg = new ImageIcon(Config.resize(Ressources.cards[cards.get(currentPos)[0]][cards.get(currentPos)[1]],width,height));
			}
			setIcon(bg);
			changeColorPoint();
			repaint();
			revalidate();
		}
	}
	
	private void changeColorPoint() {
		if(colorPoint == Color.GREEN){
			colorPoint = Color.BLUE;
		} else {
			colorPoint = Color.GREEN;
		}
	}

	public void changeBgUp(){
		if(currentPos < cards.size()-1  && cards.size() > 0){
			currentPos+=1;
			ImageIcon bg;
			if(width == 95){
				bg = new ImageIcon(Ressources.cards[cards.get(currentPos)[0]][cards.get(currentPos)[1]]);
			} else {
				bg = new ImageIcon(Config.resize(Ressources.cards[cards.get(currentPos)[0]][cards.get(currentPos)[1]],width,height));
			}
			setIcon(bg);
			changeColorPoint();
			repaint();
			revalidate();
		}
	}
	
	public void changeBgDown(){
		if(currentPos > 0 && cards.size() > 0){
			currentPos-=1;
			ImageIcon bg;
			if(width == 95){
				bg = new ImageIcon(Ressources.cards[cards.get(currentPos)[0]][cards.get(currentPos)[1]]);
			} else {
				bg = new ImageIcon(Config.resize(Ressources.cards[cards.get(currentPos)[0]][cards.get(currentPos)[1]],width,height));
			}
			setIcon(bg);
			changeColorPoint();
			repaint();
			revalidate();
		}
	}
	

	public void setEnabled(boolean b){
		enabled = b;
		if(enabled){
			
		} else {
			
		}
		revalidate();
		repaint();
	}

	/***
	 * @return si le bouton est
	 * activé ou desactivé
	 */
	public boolean isMyEnabled() {
		return enabled;
	}

	public void removeCards() {
		cards.clear();
		setIcon(null);
	}

	public int getNb() {
		return cards.size();
	}
	
	
	@Override
	public void paintComponent(Graphics g) {
		super.paintComponent(g);
		Graphics2D g2 = (Graphics2D) g;
		g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING,RenderingHints.VALUE_ANTIALIAS_ON);
		if(cards.size() != 0){
			g2.setColor(colorPoint);
			g2.fillOval(5,5,10,10);
		}
	}
	

}