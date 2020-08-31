#version 410 core
out vec4 FragColor;

varying vec2 vTexCoords;
varying vec3 vPosition;
uniform sampler2D texture_ground1;
uniform sampler2D texture_ground2;


void main()
{  
//	FragColor=vec4(70.0/255.0, 150.0/255.0, 77.0/255.0, 1.0f);
	if(vPosition.y>140)
	{
		FragColor=texture2D(texture_ground1,vTexCoords);
	}
	else
	{
		float alpha=vPosition.y/(140);
		FragColor=texture2D(texture_ground1,vTexCoords)*alpha
					+texture2D(texture_ground2,vTexCoords)*(1-alpha);
	}
  
}