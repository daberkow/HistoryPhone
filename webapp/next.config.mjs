/** @type {import('next').NextConfig} */
const nextConfig = {
    output: 'export', // Enable static HTML export
    images: {
        unoptimized: true, // Required for static export
    },
    // Rewrites for development - comment out when exporting static version
    /* async rewrites() {
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
    }, */
};

export default nextConfig;
