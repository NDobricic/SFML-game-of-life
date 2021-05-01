uniform sampler2D texture;
uniform float resX;
uniform float resY;

int value(float x, float y)
{
    return int(texture2D(texture, gl_TexCoord[0].xy + vec2(x / resX, y / resY)).r);
}

void main()
{
    int sum = value(-1, 1) +
              value( 0, 1) +
              value( 1, 1) +
              value( 1, 0) +
              value( 1,-1) +
              value( 0,-1) +
              value(-1,-1) +
              value(-1, 0);

    if (sum==3)
    {
        gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
    else if (sum== 2)
    {
        float current = float(value(0, 0));
        gl_FragColor = vec4(current, current, current, 1.0);
    }
    else
    {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
