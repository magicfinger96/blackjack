import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.JButton;

/***
 * Classe représentant les
 * boutons se trouvant au milieu
 * du plateau 
 * 
 * @author andrea
 *
 */
public class Button extends JButton {
	private String name;
	private int posX;
	private int posY;
	private Color bg;
	private int size;
	private int sizefont;
	private int wid;
	private Listener listener;
	private boolean enabled;
	
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
	public Button( String str,float sizeFont, int posX, int posY,int w) {
		name = str;
		wid = w;
		this.posX = posX;
		this.posY = posY;
		sizefont=(int) sizeFont;
		size = Config.sizeOfaString(name,Ressources.font.deriveFont(sizeFont));
		setBounds(posX,posY,w,(int)sizeFont+20);
		setFont(Ressources.font.deriveFont(sizeFont));
		bg = new Color(254,193,81);
		setFocusPainted(false);
		setBorderPainted(false);
		setPreferredSize(new Dimension(w,(int)sizeFont+20));
		
		listener = new Listener();
		enabled = true;
		
		this.addMouseListener(listener);
	}
	

	public void setEnabled(boolean b){
		enabled = b;
		if(enabled){
			bg = new Color(254,193,81);
		} else {
			bg = new Color(226,167,52);
		}
		revalidate();
		repaint();
	}
	
	/***
	 * met a jour le nom dans 
	 * le bouton
	 */
	public void setName(String newName){
		name = newName;
		size = Config.sizeOfaString(newName,Ressources.font.deriveFont((float)sizefont));
		repaint();
	}
	
	
	public void paintComponent(Graphics g){
		Graphics2D g2 = (Graphics2D) g;
		g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING,RenderingHints.VALUE_ANTIALIAS_ON);
		
		g2.setColor(bg);
		g2.fillRoundRect(0,0,getWidth(),getHeight(),15,15);
		
		
		g2.setColor(new Color(153,102,51));
		g2.drawString(name,(getWidth()-size)/2,sizefont+5);
	}
	
	private class Listener implements MouseListener {

		@Override
		public void mouseReleased(MouseEvent arg0) {
			if(enabled){
				bg = new Color(254,193,81);
			}
			
		}
		
		@Override
		public void mousePressed(MouseEvent arg0) {
			if(enabled){
				bg = new Color(226,167,52);
			}
			
		}
		
		@Override
		public void mouseExited(MouseEvent arg0) {
			if(enabled){
				bg = new Color(254,193,81);
			}
			
		}
		
		@Override
		public void mouseEntered(MouseEvent arg0) {
			if(enabled){
				bg = new Color(255,211,126);
			}
		}
		
		@Override
		public void mouseClicked(MouseEvent arg0) {
			
		}
		
	}

	/***
	 * @return si le bouton est
	 * activé ou desactivé
	 */
	public boolean isMyEnabled() {
		return enabled;
	}

}