/** @type {import('next').NextConfig} */
const nextConfig = {
    async rewrites() {
        return [
            {
                source: '/api/:path*',
                destination: 'http://192.168.4.1/api/:path*', // Proxy to your API
            },
            {
                source: '/content/:path*',
                destination: 'http://192.168.4.1/content/:path*', // Proxy to your API
            },
        ];
    },
};

export default nextConfig;
