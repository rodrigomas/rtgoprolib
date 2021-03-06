        ��  ��                  �  (   T E X T   ��e       0 	        uniform float dx;
uniform float dy;
uniform float w;
uniform float h;

uniform float bend_ur;
uniform float bend_dr;
uniform float bend_dl;
uniform float bend_ul;

uniform float bend_left;
uniform float bend_right;

uniform float FlipY;

uniform float UsePolar;

const float PI = 3.1415926535;

vec2 Distort(vec2 p, float d)
{
    vec2 v = p.xy;
    
	if(UsePolar > 0.0)
	{
		float radius = length(v);
		if (radius > 0.0)
		{
		  float theta = atan(v.y,v.x);
      
		  radius = pow(radius, d * 0.01 + 1.0);
  
		  v.x = radius * cos(theta);
		  v.y = radius * sin(theta);
		  p.xy = v.xy;
		}
	
		return 0.5 * (p + 1.0);
	} else 
	{
		v.y = v.y + (v.x * v.x) * d * v.y;
	}	

	if (p.x < 0.0)
	{
		v.x = v.x + bend_left * (p.y * p.y) * v.x;
	} else 
	{
		v.x = v.x + bend_right * (p.y * p.y) * v.x;
	}	

	return 0.5 * (v + 1.0);
}

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;

	if(FlipY > 0.0)
		gl_TexCoord[0].t = 1.0 - gl_TexCoord[0].t;
	
	vec2 tex = 2.0 * (gl_MultiTexCoord0.xy - 0.5);
	vec4 vt = gl_Vertex;

	if( tex.x > 0.0 )
	{
		if( tex.y > 0.0 )  //Reversed
		{
			vt.xy = Distort(tex, bend_dr);	
		}
		else
		{
			vt.xy = Distort(tex, bend_ur);
		}
		
	} 
	else
	{
		if( tex.y > 0.0 )
		{
			vt.xy = Distort(tex, bend_dl);
		} else 
		{
			vt.xy = Distort(tex, bend_ul);
		}		
	}	

	vt.x = vt.x * w + dx;
	vt.y = vt.y * h + dy;

	gl_FrontColor = gl_Color;

	gl_Position = gl_ModelViewProjectionMatrix * vt;
}
    (   T E X T   ��f       0 	        uniform sampler2D renderedTexture;
uniform float w;
uniform float h;

uniform  float FadeRight;
uniform  float FadeLeft;
uniform  float FadeRightPower;
uniform  float FadeLeftPower;
uniform  float Gamma;
uniform  float VerticalFade;

uniform  float OffsetTX;
uniform  float OffsetTY;

uniform float RotateImg;
uniform float FlipRGB;

float remaper_left(float val)
{
	//return 1.0 - smoothstep(0, 1, val / FadeLeft);
	return val / (FadeLeft);
}

float remaper_right(float val)
{
	//return 1.0 - smoothstep(0, 1, val / FadeRight);
	return val / (1.0 - FadeRight);
}

float border_factor(float t)
{
	float f = abs(t - 0.5) * 2.0;

	return (f > VerticalFade) ? ( 1.0 - smoothstep(0.0, 1.0 - VerticalFade, (f - VerticalFade))) : 1.0;
}

float bezier(float t, float power)
{
	return pow(1.0 - t, power);
	//return pow(1.0 - t, 1);
}

 void main()
 {
	vec2 tex = gl_TexCoord[0].st / 2.0;
	
	if(RotateImg < -89.0)
		tex = tex.ts;
	else if(RotateImg > 89.0)
		tex = vec2(tex.t, 0.5 - tex.s);
	
	tex += vec2(OffsetTX, OffsetTY);

	vec4 color = texture2D(renderedTexture, tex) * gl_Color;

	if(FlipRGB > 0.0)
		color.rgb = color.bgr;

	float alpha = (gl_TexCoord[0].x < FadeLeft) ? bezier(remaper_left(FadeLeft - gl_TexCoord[0].x), FadeLeftPower) : 1.0;

	alpha = (gl_TexCoord[0].x > FadeRight) ? bezier(remaper_right(gl_TexCoord[0].x - FadeRight), FadeRightPower) : alpha;

	alpha = alpha * border_factor(gl_TexCoord[0].y);

	color.rgb = color.rgb * Gamma;

	color.a *= alpha;

	gl_FragColor = color;
 }
�      �� ��     0 	        �4   V S _ V E R S I O N _ I N F O     ���                 ?                         D   S t r i n g F i l e I n f o       0 4 0 9 0 4 b 0   0   C o m p a n y N a m e     V F X   R & D   `   F i l e D e s c r i p t i o n     R e a l t i m e   G o P r o   S t i t c h e r   G P U   0   F i l e V e r s i o n     0 . 0 . 0 . 3   >   I n t e r n a l N a m e   r t g o p r o l i b . d l l     J   L e g a l C o p y r i g h t   C o p y r i g h t   ( C )   2 0 1 6     F   O r i g i n a l F i l e n a m e   r t g o p r o l i b . d l l     @   P r o d u c t N a m e     R T G o P r o   L i b r a r y   4   P r o d u c t V e r s i o n   0 . 0 . 0 . 3   D    V a r F i l e I n f o     $    T r a n s l a t i o n     	�