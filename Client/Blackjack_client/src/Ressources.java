import java.awt.Font;
import java.awt.FontFormatException;
import java.awt.image.BufferedImage;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.ImageIcon;

public class Ressources {
	static BufferedImage bg;
	static BufferedImage bgGame;
	static BufferedImage footer;
	static BufferedImage bar;
	static Font font;
	static BufferedImage arrowBackHovered;
	static BufferedImage arrowBackPressed;
	static BufferedImage arrowBack;
	static BufferedImage refreshHovered;
	static BufferedImage refresh;
	static BufferedImage tken;
	static BufferedImage refreshPressed;
	static BufferedImage[][] cards;
	
	/***
	 * Constructeur de Ressources
	 * Les ressources y sont chargées.
	 */
	public Ressources() {
		try {
			bg = ImageIO.read(getClass().getResource( "/ressources/images/bg1.png"));
			tken = ImageIO.read(getClass().getResource( "/ressources/images/token.png"));
			footer = ImageIO.read(getClass().getResource( "/ressources/images/footer.png"));
			bar = ImageIO.read(getClass().getResource( "/ressources/images/bar.png"));
			bgGame = ImageIO.read(getClass().getResource( "/ressources/images/bg2.png"));
			font = Font.createFont(Font.TRUETYPE_FONT, getClass().getResourceAsStream("/ressources/polices/font.ttf"));
			arrowBackHovered =ImageIO.read(getClass().getResource( "/ressources/images/arrowBackHovered.png"));
			arrowBack= ImageIO.read(getClass().getResource( "/ressources/images/arrowBack.png"));
			arrowBackPressed= ImageIO.read(getClass().getResource( "/ressources/images/arrowBackPressed.png"));
			refreshHovered =ImageIO.read(getClass().getResource( "/ressources/images/refreshHovered.png"));
			refresh= ImageIO.read(getClass().getResource( "/ressources/images/refresh.png"));
			refreshPressed= ImageIO.read(getClass().getResource( "/ressources/images/refreshPressed.png"));
			
			
			cards = new BufferedImage[4][13];
			int i,j;
			for(i=0;i<4;i++){
				for(j=0;j<13;j++){
					cards[i][j]= ImageIO.read(getClass().getResource( "/ressources/images/cards/"+(i+1)+"/"+(j+1)+".png"));;
				}
			}
			
		} catch (IOException | FontFormatException e) {
			e.printStackTrace();
		}
	}
}